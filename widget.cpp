#include "widget.h"
#include "ui_widget.h"
#include "settings.h"
#include "settingswidget.h"
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

int Widget::computeFitness(const QImage& target, const QRect box)
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
    for (int i=0; i < N_CORES; i++){
        slices[i] = QtConcurrent::run(computeSlice, miny+(maxy/N_CORES) *i, (maxy/N_CORES) * (i+1));
    }
	unsigned fitness=0;
    for (int i=0; i < N_CORES; i++)
        fitness+=slices[i].result();
    return fitness;
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
    }
    painter.end();
    return predrawn;
}

Poly Widget::genPoly()
{
    Poly poly;
    for (int i=0; i<N_POLY_POINTS; i++)
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
    poly.color.setAlpha(qrand()%180+20);
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
    //poly.color = pic.pixel(avgx,avgy);
    poly.color.setAlpha(qrand()%180+20);
#endif
    return poly;
}

void Widget::redraw(QImage& target, QVector<Poly> &polyList)
{
    static QBrush brush(Qt::SolidPattern);
    target.fill(Qt::white);
    QPainter painter(&target);
    painter.setPen(QPen(Qt::NoPen));
    for (Poly& poly : polyList)
    {
        brush.setColor(poly.color);
        painter.setBrush(brush);
        painter.drawPolygon(poly.points.data(), poly.points.size());
    }
}

void Widget::run()
{
    // Main loop
    while (running)
    {
        int polysSize = polys.size();

        // Always keep a minimum number of polys
        if (polysSize < POLYS_MIN && polysSize < POLYS_MAX)
        {
            tryAddPoly();
            continue;
        }

        // Mutate polygons
        int oldFit = fitness; // Because tryAddPoly will change it directly
        if (qrand()%POLYS_ADD_RATE==0 && polysSize < POLYS_MAX)
            tryAddPoly(); // Will modify generated directly if it suceeds

        QImage newGen = generated;
        QVector<Poly> polysNew = polys;
        bool dirty=false;

        if (qrand()%POLYS_REMOVE_RATE==0 && polysSize > POLYS_MIN)
        {
            removePoly(polysNew);
            dirty=true;
        }

        if (qrand()%POLYS_REORDER_RATE==0)
        {
            reorderPoly(polysNew);
            dirty=true;
        }

        if (dirty)
        {
            // Keep polys improvements
            redraw(newGen, polysNew);
            int newFit = computeFitness(newGen);
            if (newFit <= oldFit)
            {
                polys = polysNew;
                generated = newGen;
                fitness = newFit;
                updateGuiFitness();
                ui->polysLabel->setNum(polys.size());
                ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            }
        }
        generation++;
        ui->generationLabel->setNum(generation);
        app->processEvents();

        // Mutate points separately
        // (it's unlikely that changing both the polys and a random point will help)
        newGen = generated;
        polysNew = polys;
        dirty = false;


        for (Poly& poly : polysNew)
        {
            for (QPoint& point : poly.points)
            {
                if (qrand()%POINT_MOVE_MAX_RATE==0)
                {
                    point.rx() = qrand()%width;
                    point.ry() = qrand()%height;
                    dirty = true;
                }
                else if (qrand()%POINT_MOVE_MED_RATE==0)
                {
                    point.rx() += qrand()%(POINT_MOVE_MED_RANGE)-POINT_MOVE_MED_RANGE/2;
                    point.rx() = min((unsigned)max(point.x(), 0), width);
                    point.ry() += qrand()%(POINT_MOVE_MED_RANGE)-POINT_MOVE_MED_RANGE/2;
                    point.ry() = min((unsigned)max(point.y(), 0), height);
                    dirty = true;
                }
                else if (qrand()%POINT_MOVE_MIN_RATE==0)
                {
                    point.rx() += qrand()%(POINT_MOVE_MIN_RANGE)-POINT_MOVE_MIN_RANGE/2;
                    point.rx() = min((unsigned)max(point.x(), 0), width);
                    point.ry() += qrand()%(POINT_MOVE_MIN_RANGE)-POINT_MOVE_MIN_RANGE/2;
                    point.ry() = min((unsigned)max(point.y(), 0), height);
                    dirty = true;
                }
            }
        }

        if (dirty)
        {
            // Keep points improvements
            redraw(newGen, polysNew);
            int newFit = computeFitness(newGen);
            if (newFit <= oldFit)
            {
                polys = polysNew;
                generated = newGen;
                fitness = newFit;
                updateGuiFitness();
                ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            }
        }
        generation++;
        ui->generationLabel->setNum(generation);
        app->processEvents();
    }
}
