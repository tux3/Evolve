#include "widget.h"
#include "ui_widget.h"
#include "settings.h"
#include "settingswidget.h"
#include "stats.h"
#include <QDataStream>
#include <QString>
#include <QPixmap>
#include <QPen>
#include <QPainter>
#include <QRgb>
#include <QtConcurrent/QtConcurrent>
#include <ctime>

unsigned Widget::height;
unsigned Widget::width;
QImage Widget::pic;
QVector<Poly> Widget::polys;

using namespace std;

Widget::Widget() :
    QWidget(nullptr),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Build the menu
    menuBar = new QMenuBar(this);
    QMenu* fileMenu = menuBar->addMenu(tr("&File"));
    openAction = fileMenu->addAction("&Open image", this, SLOT(openImageClicked()));
    fileMenu->addAction("&Save image", this, SLOT(saveImageClicked()));
    fileMenu->addAction("&Export as SVG", this, SLOT(saveSVGClicked()));
    fileMenu->addSeparator();
    startStopAction = fileMenu->addAction("S&tart");
    fileMenu->addAction("&Quit", this, SLOT(close()));
    QMenu* dnaMenu = menuBar->addMenu(tr("&DNA"));
    dnaMenu->addAction("&Import DNA", this, SLOT(importDnaClicked()));
    dnaMenu->addAction("&Export DNA", this, SLOT(exportDnaClicked()));
    cleanAction = dnaMenu->addAction("&Clean DNA", this, SLOT(cleanDnaClicked()));
    optimizeAction = dnaMenu->addAction("&Optimize DNA", this, SLOT(optimizeDnaClicked()));
    QMenu* settingsMenu = menuBar->addMenu(tr("&Settings"));
    settingsMenu->addAction("&Settings", this, SLOT(settingsClicked()));
    QMenu* helpMenu = menuBar->addMenu(tr("&?"));
    helpMenu->addAction("&Focus", this, SLOT(focusClicked()));
    helpMenu->addAction("&GitHub page", this, SLOT(githubClicked()));
    helpMenu->addAction("&Stats", this, SLOT(statsClicked()));
    ui->gridLayout->addWidget(menuBar,0,0,1,4);
    menuBar->setFixedHeight(22);

    generation = 0;
    running = false;

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openImageClicked()));
    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(startClicked()));

    ui->imgOriginal->installEventFilter(this);

    qsrand(time(NULL));
}

Widget::~Widget()
{
    delete ui;
    running = false;
    exit(0);
}

unsigned Widget::computeFitness(const QImage& target, const QRect box)
{
    unsigned minx, maxx, miny, maxy;
    if (box.isNull())
    {
        minx = miny = 0;
        maxx = width;
        maxy = height;
    }
    else
    {
        minx = box.x();
        miny = box.y();
        maxx = minx + box.width();
        maxy = miny + box.height();
    }

    static QVector<QRgb*> originalLines;
    originalLines.resize(maxy-miny);
    for (unsigned i=miny; i<maxy; i++)
        originalLines[i] = ((QRgb*)pic.scanLine(i));
    static QVector<QRgb*> targetLines;
    targetLines.resize(maxy-miny);
    for (unsigned i=miny; i<maxy; i++)
        targetLines[i] = ((QRgb*)target.scanLine(i));

    auto computeSlice = [&](const unsigned start, const unsigned end)
    {
        unsigned partFitness=0;
        for (unsigned i=start-miny; i<end-miny; i++)
        {
            // Sum of the differences of each pixel's color
            for (unsigned j=minx; j<maxx; j++)
            {
                unsigned ocolor = originalLines.at(i)[j];
                int oR=(ocolor>>16), oG=(ocolor>>8)&0xFF, oB=(ocolor&0xFF);
                unsigned tcolor = targetLines.at(i)[j];
                int tR=(tcolor>>16), tG=(tcolor>>8)&0xFF, tB=(tcolor&0xFF);
                partFitness += abs(tR-oR)+abs(tG-oG)+abs(tB-oB);
            }
        }
        return partFitness;
    };
    QFuture<unsigned> slices[N_CORES];
    for (unsigned i=0; i < N_CORES; i++){
        slices[i] = QtConcurrent::run(computeSlice, miny+(maxy/N_CORES) *i, (maxy/N_CORES) * (i+1));
    }
    unsigned fullFitness=0;
    for (unsigned i=0; i < N_CORES; i++)
        fullFitness+=slices[i].result();
    return fullFitness;
}

QImage Widget::predraw(int polyIndex)
{
    QImage predrawn(width, height, QImage::Format_ARGB32);
    predrawn.fill(Qt::white);
    static QBrush brush(Qt::SolidPattern);
    QPainter painter(&predrawn);
    painter.setPen(QPen(Qt::NoPen));
    for (int i=0; i<polyIndex; ++i)
    {
        brush.setColor(polys[polyIndex].color);
        painter.setBrush(brush);
        painter.drawPolygon(polys[polyIndex].points.data(), polys[polyIndex].points.size());
#if (useConvexPolys)
    painter.drawConvexPolygon(polys[polyIndex].points.data(), polys[polyIndex].points.size());
#else
    painter.drawPolygon(polys[polyIndex].points.data(), polys[polyIndex].points.size());
#endif
    }
    painter.end();
    return predrawn;
}

Poly Widget::genPoly()
{
    Poly poly;
    for (unsigned i=0; i<N_POLY_POINTS; i++)
    {
        quint16 x,y;
        int wMod = (int)(((float)width*(float)(FOCUS_RIGHT-FOCUS_LEFT))/100.0);
        int hMod = (int)(((float)height*(float)(FOCUS_BOTTOM-FOCUS_TOP))/100.0);
        x = qrand()%wMod;
        x += (width*(float)FOCUS_LEFT/100.0);
        y = qrand()%hMod;
        y += (height*(float)FOCUS_TOP/100.0);
        poly.points.append(QPoint(x,y));
    }
#if GEN_WITH_RANDOM_COLOR
    poly.color = QColor::fromRgb(qrand()*qrand()*qrand());
    poly.color.setAlpha(qrand()%(ALPHA_MAX-ALPHA_MIN)+ALPHA_MIN);
#else
    quint64 avgx=0, avgy=0;
    for (QPoint point : poly.points)
    {
        avgx += point.x();
        avgy += point.y();
    }
    avgx /= N_POLY_POINTS;
    avgy /= N_POLY_POINTS;

    poly.color = pic.pixel(avgx,avgy);
    poly.color.setAlpha(qrand()%(ALPHA_MAX-ALPHA_MIN)+ALPHA_MIN);
#endif
    return poly;
}

void Widget::redraw(QImage& target, QVector<Poly> &polyList, unsigned startFrom)
{
    static QBrush brush(Qt::SolidPattern);
    if (startFrom == 0)
        target.fill(Qt::white);
    QPainter painter(&target);
    painter.setPen(QPen(Qt::NoPen));
    unsigned size = polyList.size();
    for (unsigned i=startFrom; i<size; ++i)
    {
        Poly& poly = polyList[i];
        brush.setColor(poly.color);
        painter.setBrush(brush);
#if (useConvexPolys)
    painter.drawConvexPolygon(poly.points.data(), poly.points.size());
#else
    painter.drawPolygon(poly.points.data(), poly.points.size());
#endif
    }
}

bool Widget::isInFocus(const Poly& poly)
{
    if (FOCUS_LEFT == 0 && FOCUS_TOP == 0
        && FOCUS_BOTTOM == 100 && FOCUS_RIGHT == 100)
    {
        return true;
    }
    for (QPoint point : poly.points)
    {
        if ((unsigned)point.x() < FOCUS_LEFT*width/100
            || (unsigned)point.x() > FOCUS_RIGHT*width/100
            || (unsigned)point.y() < FOCUS_TOP*height/100
            || (unsigned)point.y() > FOCUS_BOTTOM*height/100)
        {
            return false;
        }
    }
    return true;
}

void Widget::run()
{
    qsrand(time(NULL)); // Just in case the user was having bad random, here's a way to reset

    // Main loop
    while (running)
    {
        unsigned polysSize = polys.size();

        // Always keep a minimum number of polys
        if (polysSize < POLYS_MIN && polysSize < POLYS_MAX)
        {
            tryAddPoly();
            continue;
        }

        // Mutate polygons
        polysSize=polys.size();
        for (unsigned i=0; i<polysSize && (unsigned)polys.size() < POLYS_MAX; ++i)
            if (qrand()%POLYS_ADD_RATE==0)
                tryAddPoly(); // Will modify generated directly if it suceeds

        QImage newGen;
        QVector<Poly> polysNew=polys;
        QImage predraw(width, height, QImage::Format_ARGB32);
        predraw.fill(Qt::white);
        for (unsigned i=0; i<(unsigned)polysNew.size() && (unsigned)polysNew.size() > POLYS_MIN; ++i)
        {
            bool dirty=false;
            bool removedPoly=false;
            polysNew = polys;
            if (qrand()%POLYS_REMOVE_RATE==0 && (unsigned)polysNew.size() > POLYS_MIN)
            {
                removedPoly=true;
                STAT_POLY_REMOVE++;
                removePoly(polysNew);
                dirty=true;
            }

            if (dirty)
            {
                // Keep polys improvements
                newGen = predraw;
                if (i<(unsigned)polysNew.size())
                    redraw(newGen, polysNew, i);
                int newFit = computeFitness(newGen);
                if (newFit <= fitness)
                {
                    polys = polysNew;
                    generated = newGen;
                    fitness = newFit;
                    updateGuiFitness();
                    ui->polysLabel->setNum(polys.size());
                    ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                    if (removedPoly)
                        STAT_POLY_REMOVE_OK++;
                }
            }
            if (i<(unsigned)polysNew.size())
                Poly::drawPoly(predraw, polysNew[i]);
            generation++;
            ui->generationLabel->setNum(generation);
            app->processEvents();
        }

        predraw.fill(Qt::white);
        for (unsigned i=0; i<polysSize; ++i)
        {
            newGen = generated;
            polysNew = polys;
            bool dirty = false;
            bool reorderedPoly=false;
            if (qrand()%POLYS_REORDER_RATE==0)
            {
                reorderedPoly=true;
                STAT_POLY_REORDER++;
                reorderPoly(polysNew);
                dirty=true;
            }

            if (dirty)
            {
                // Keep polys improvements
                redraw(newGen, polysNew);
                int newFit = computeFitness(newGen);
                if (newFit <= fitness)
                {
                    polys = polysNew;
                    generated = newGen;
                    fitness = newFit;
                    updateGuiFitness();
                    ui->polysLabel->setNum(polys.size());
                    ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                    if (reorderedPoly)
                        STAT_POLY_REORDER_OK++;
                }
            }
            generation++;
            ui->generationLabel->setNum(generation);
            app->processEvents();
        }

        // Mutate colors separately
        polysNew = polys;
        bool dirty = false;
        predraw.fill(Qt::white);
        unsigned polysNewSize = polysNew.size();
        for (unsigned i=0; i<polysNewSize; ++i)
        {
            Poly& poly = polysNew[i];
            if (!isInFocus(poly))
                continue;
            if (qrand()%RED_CHANGE_RATE==0)
            {
                poly.color.setRed(qrand()%255);
                dirty=true;
            }
            if (qrand()%GREEN_CHANGE_RATE==0)
            {
                poly.color.setGreen(qrand()%255);
                dirty=true;
            }
            if (qrand()%BLUE_CHANGE_RATE==0)
            {
                poly.color.setBlue(qrand()%255);
                dirty=true;
            }
            if (qrand()%ALPHA_CHANGE_RATE==0)
            {
                poly.color.setAlpha(qrand()%(ALPHA_MAX-ALPHA_MIN)+ALPHA_MIN);
                dirty=true;
            }
            if (dirty)
            {
                // Keep color improvements
                newGen=predraw;
                redraw(newGen, polysNew, i);
                int newFit = computeFitness(newGen);
                if (newFit <= fitness)
                {
                    polys[i] = poly;
                    generated = newGen;
                    fitness = newFit;
                    updateGuiFitness();
                    ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                }
                else
                    polysNew[i] = polys[i];
                dirty = false;
            }
            Poly::drawPoly(predraw, polysNew[i]);
            generation++;
            ui->generationLabel->setNum(generation);
            app->processEvents();
        }

        // Mutate points separately
        // (it's unlikely that changing both the polys and a random point will help)
        newGen = generated;
        polysNew = polys;
        dirty = false;

        polysNewSize = polysNew.size();
        predraw.fill(Qt::white);
        for (unsigned i=0; i<polysNewSize; ++i)
        {
            Poly& poly = polysNew[i];
            if (!isInFocus(poly))
                continue;
            int movedPointMin=0, movedPointMed=0, movedPointMax=0;
            for (QPoint& point : poly.points)
            {
                if (qrand()%POINT_MOVE_MAX_RATE==0)
                {
                    point.rx() = qrand()%width;
                    point.ry() = qrand()%height;
                    dirty = true;
                    movedPointMax++;
                    STAT_POINT_MOVE_MAX++;
                }
                else if (qrand()%POINT_MOVE_MED_RATE==0)
                {
                    point.rx() += qrand()%(POINT_MOVE_MED_RANGE)-POINT_MOVE_MED_RANGE/2;
                    point.rx() = min((unsigned)max(point.x(), 0), width);
                    point.ry() += qrand()%(POINT_MOVE_MED_RANGE)-POINT_MOVE_MED_RANGE/2;
                    point.ry() = min((unsigned)max(point.y(), 0), height);
                    dirty = true;
                    movedPointMed++;
                    STAT_POINT_MOVE_MED++;
                }
                else if (qrand()%POINT_MOVE_MIN_RATE==0)
                {
                    point.rx() += qrand()%(POINT_MOVE_MIN_RANGE)-POINT_MOVE_MIN_RANGE/2;
                    point.rx() = min((unsigned)max(point.x(), 0), width);
                    point.ry() += qrand()%(POINT_MOVE_MIN_RANGE)-POINT_MOVE_MIN_RANGE/2;
                    point.ry() = min((unsigned)max(point.y(), 0), height);
                    dirty = true;
                    movedPointMin++;
                    STAT_POINT_MOVE_MIN++;
                }
            }
            polysNew[i] = poly;
            if (dirty)
            {
                // Keep points improvements
                newGen=predraw;
                redraw(newGen, polysNew, i);
                int newFit = computeFitness(newGen);
                if (newFit <= fitness)
                {
                    polys[i] = poly;
                    generated = newGen;
                    fitness = newFit;
                    updateGuiFitness();
                    ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                    if (movedPointMax)
                        STAT_POINT_MOVE_MAX_OK+=movedPointMax;
                    if (movedPointMed)
                        STAT_POINT_MOVE_MED_OK+=movedPointMed;
                    if (movedPointMin)
                        STAT_POINT_MOVE_MIN_OK+=movedPointMin;
                }
                dirty=false;
            }
            generation++;
            ui->generationLabel->setNum(generation);
            app->processEvents();
            Poly::drawPoly(predraw, polysNew[i]);
        }
    }
}
