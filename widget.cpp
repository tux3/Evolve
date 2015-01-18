#include "widget.h"
#include "ui_widget.h"
#include "settings.h"
#include "settingswidget.h"
#include "analysis.h"
#include <QDataStream>
#include <QString>
#include <QPixmap>
#include <QPen>
#include <QPainter>
#include <QRgb>
#include <QtConcurrent/QtConcurrent>
#include <ctime>

//#define TIME_FITNESS

#ifdef TIME_FITNESS
#include <QElapsedTimer>
#endif

unsigned Widget::height;
unsigned Widget::width;
QImage Widget::pic;
QVector<Poly> Widget::polys;
QTimer Widget::autofocusTimer;

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
    fileMenu->addAction("&AutoFocus", this, SLOT(autofocusClicked()));
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
    helpMenu->addAction("&Redraw", this, SLOT(redrawClicked()));
    helpMenu->addAction("&GitHub page", this, SLOT(githubClicked()));
    ui->gridLayout->addWidget(menuBar,0,0,1,4);
    menuBar->setFixedHeight(22);

    autofocusTimer.setSingleShot(false);
    autofocusTimer.setInterval(AUTOFOCUS_DELAY);

    generation = 0;
    running = false;

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openImageClicked()));
    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(startClicked()));
    connect(&autofocusTimer, SIGNAL(timeout()), this, SLOT(autofocusClicked()));

    ui->imgOriginal->installEventFilter(this);

    qsrand(time(NULL));
}

Widget::~Widget()
{
    delete ui;
    running = false;
    exit(0);
}

void Widget::setAutofocus(bool enabled)
{
    if (enabled)
    {
        autofocusTimer.start(AUTOFOCUS_DELAY);
    }
    else
    {
        autofocusTimer.stop();
        autofocusTimer.setInterval(AUTOFOCUS_DELAY);
    }
}

QRect Widget::computeAutofocusFitness(const QImage& target)
{
    static QVector<QRgb*> originalLines;
    originalLines.resize(height);
    for (unsigned i=0; i<height; i++)
        originalLines[i] = ((QRgb*)pic.scanLine(i));
    static QVector<QRgb*> targetLines;
    targetLines.resize(height);
    for (unsigned i=0; i<height; i++)
        targetLines[i] = ((QRgb*)target.scanLine(i));

    auto computeSlice = [&](const unsigned minx, const unsigned maxx, const unsigned miny, const unsigned maxy)
    {
        unsigned partFitness=0;
        for (unsigned i=miny; i<maxy; i++)
        {
            // Sum of the differences of each pixel's color
            for (unsigned j=minx; j<maxx; j++)
            {
                unsigned ocolor = originalLines.at(i)[j];
                int oR=(ocolor>>16)&0xFF, oG=(ocolor>>8)&0xFF, oB=(ocolor&0xFF);
                unsigned tcolor = targetLines.at(i)[j];
                int tR=(tcolor>>16)&0xFF, tG=(tcolor>>8)&0xFF, tB=(tcolor&0xFF);
                partFitness += abs(tR-oR)+abs(tG-oG)+abs(tB-oB);
            }
        }
        return partFitness;
    };

    QFuture<unsigned> slices[AUTOFOCUS_SUBDIVS*AUTOFOCUS_SUBDIVS];
    unsigned const sliceW = width/AUTOFOCUS_SUBDIVS, sliceH = height/AUTOFOCUS_SUBDIVS;
    for (int i=0; i < AUTOFOCUS_SUBDIVS*AUTOFOCUS_SUBDIVS; i++){
        slices[i] = QtConcurrent::run(computeSlice,
                        (i%AUTOFOCUS_SUBDIVS)*sliceW, (i%AUTOFOCUS_SUBDIVS+1)*sliceW,
                        (i/AUTOFOCUS_SUBDIVS)*sliceH, (i/AUTOFOCUS_SUBDIVS+1)*sliceH);
    }
    quint64 worstFitness = sliceW*sliceH*3*255;
    int best=0;
    float min=100;
    for (int i=0; i < AUTOFOCUS_SUBDIVS*AUTOFOCUS_SUBDIVS; i++)
    {
        float percentFitness = 100.0-((double)slices[i].result()/(double)worstFitness*100.0);
        if (percentFitness<min)
        {
            min=percentFitness;
            best=i;
        }
    }
    return QRect(best%AUTOFOCUS_SUBDIVS*sliceW,best/AUTOFOCUS_SUBDIVS*sliceH,sliceW,sliceH);
}


quint64 Widget::computeFitness(const QImage& target)
{
#ifdef TIME_FITNESS
    QElapsedTimer timer;
    timer.start();
#endif

    const uchar* origData = pic.bits(), *targetData = target.bits();
    const unsigned BPL = pic.bytesPerLine();

    auto computeSlice = [&](const unsigned start, const unsigned end)
    {
        quint64 partFitness=0;
        __m64 mmFitness = _mm_setzero_si64();
        __m64 tmp;
        for (unsigned i=start; i<end; i++)
        {
            const unsigned curLine = BPL*i;
            // Sum of the differences of each pixel's color
            for (unsigned j=0; j+8<BPL; j+=8)
            {
                __m64 mmOrig = _m_from_int64(*(quint64*)(origData+curLine+j));
                __m64 mmTarget = _m_from_int64(*(quint64*)(targetData+curLine+j));
                tmp = _m_psadbw(mmOrig, mmTarget);
                mmFitness = _mm_add_si64(mmFitness, tmp);
            }
        }
        *(__m64*)(&partFitness) += mmFitness;
        return partFitness;
    };
    QFuture<quint64> slices[N_CORES];
    for (int i=0; i < N_CORES; i++){
        slices[i] = QtConcurrent::run(computeSlice, height/N_CORES*i, height/N_CORES*(i+1));
    }
    quint64 fitness=0;
    for (int i=0; i < N_CORES; i++)
        fitness+=slices[i].result();

#ifdef TIME_FITNESS
    static quint64 elapsed=0, runs=0;
    elapsed += timer.nsecsElapsed();
    runs++;
    qDebug() << "Fitness:" << elapsed/runs/1000;
#endif

    return fitness;
}

QImage Widget::predraw(int polyIndex)
{
    QImage predrawn(width, height, QImage::Format_RGB32);
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

    if (avgx>0&&avgx<width
        &&avgy>0&&avgy<height)
    {
        QColor t,b,l,r,c;
        t = pic.pixel(avgx,avgy-1);
        b = pic.pixel(avgx,avgy+1);
        l = pic.pixel(avgx-1,avgy);
        r = pic.pixel(avgx+1,avgy);
        c = pic.pixel(avgx,avgy);
        poly.color = QColor((t.red()+b.red()+l.red()+r.red()+c.red())/5,
                            (t.green()+b.green()+l.green()+r.green()+c.green())/5,
                            (t.blue()+b.blue()+l.blue()+r.blue()+c.blue())/5);
    }
    else
    {
        poly.color = pic.pixel(avgx,avgy);
    }
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

        if (!polysSize)
        {
            QColor bg = findDominantColor(pic);
            Poly poly;
            poly.color = bg;
            poly.points.append(QPoint(0, 0));
            poly.points.append(QPoint(width, 0));
            poly.points.append(QPoint(width, height));
            poly.points.append(QPoint(0, height));
            polys.append(poly);
            Poly::drawPoly(generated, poly);
            ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            ui->polysLabel->setNum(1);
        }

        // Lower the number of points progressively to get more details
        if (polysSize == 10 && SettingsWidget::isDefaultConfig)
            N_POLY_POINTS = 5;
        else if (polysSize == 25 && SettingsWidget::isDefaultConfig)
            N_POLY_POINTS = 4;
        else if (polysSize == 50 && SettingsWidget::isDefaultConfig)
            N_POLY_POINTS = 3;

        // Always keep a minimum number of polys
        if (polysSize < POLYS_MIN && polysSize < POLYS_MAX)
        {
            tryAddPoly();
            generation++;
            ui->generationLabel->setNum(generation);
            app->processEvents();
            continue;
        }

        // Mutate
        int oldFit = fitness; // Because tryAddPoly will change it directly
        if (qrand()%100 < POLYS_ADD_RATE && polysSize < POLYS_MAX)
            tryAddPoly(); // Will modify generated directly if it suceeds

        QImage newGen = generated;
        QVector<Poly> polysNew = polys;

        if (qrand()%100 < POLYS_REMOVE_RATE && polysSize > POLYS_MIN)
            removePoly(polysNew, newGen);

        if (qrand()%100 < POLYS_REORDER_RATE)
            reorderPoly(polysNew, newGen);

        if (qrand()%100 < POINT_MOVE_RATE)
            movePoint(polysNew, newGen);

        // Keep improvements
        int newFit = computeFitness(newGen);
        if (newFit <= oldFit)
        {
            polys = polysNew;
            generated = newGen;
            fitness = newFit;
            updateGuiFitness();
        }
        generation++;
        ui->generationLabel->setNum(generation);
        ui->polysLabel->setNum(polys.size());
        app->processEvents();
    }
}
