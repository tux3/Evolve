#include "widget.h"
#include "ui_widget.h"
#include "progressdialog.h"
#include "settings.h"
#include <QDataStream>
#include <QString>
#include <QPixmap>
#include <QPen>
#include <QPainter>
#include <QRgb>
#include <QtConcurrent/QtConcurrent>
#include <QInputDialog>
#include <ctime>

unsigned Widget::height;
unsigned Widget::width;

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

int Widget::computeFitness(QImage& target, QRect box)
{
    QAtomicInt fitness = 0;

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

    QVector<QRgb*> targetLines;
    QVector<QRgb*> originalLines;
    for (unsigned i=miny; i<maxy; i++)
        originalLines.append((QRgb*)pic.scanLine(i));
    for (unsigned i=miny; i<maxy; i++)
        targetLines.append((QRgb*)target.scanLine(i));

    auto computeSlice = [&](unsigned start, unsigned end)
    {
        for (unsigned i=start-miny; i<end-miny; i++)
        {
            // Sum of the differences of each pixel's color
            for (unsigned j=minx; j<maxx; j++)
            {
                int tR,tG,tB;
                int oR,oG,oB;
                QColor(targetLines.at(i)[j]).getRgb(&tR,&tG,&tB);
                QColor(originalLines.at(i)[j]).getRgb(&oR,&oG,&oB);
                unsigned diff = abs(tR-oR)+abs(tG-oG)+abs(tB-oB);
                fitness.fetchAndAddRelaxed(diff);
            }
        }
    };
    QFuture<void> slices[N_CORES];
    for (int i=0; i < N_CORES; i++){
        slices[i] = QtConcurrent::run(computeSlice, miny+(maxy/N_CORES) *i, (maxy/N_CORES) * (i+1));
    }
    for (int i=0; i < N_CORES; i++)
        slices[i].waitForFinished();
    return fitness.load();
}

void Widget::run()
{
    unsigned worstFitness = width*height*3*255;
    int fitnessThreshold = 0.00005*((double)worstFitness)/100.0;

    // Main loop
    while (running)
    {
        if (polys.size() > 10 && SHAPE_OPT_FREQ != 0 && qrand()%SHAPE_OPT_FREQ == 0)
        {
            int i = qrand()%polys.size();
            optimizeShape(generated, polys[i], true);
        }
        else
        {

            Poly poly = genPoly();
            QImage newGen = generated;
            drawPoly(newGen, poly);
            generation++;
            int newFit = computeFitness(newGen);
            if (newFit + fitnessThreshold < fitness)
            {
                // Update data
                polys.append(poly);
                QImage clean = generated;
                generated = newGen;

                // Optimize
                optimizeColors(clean, polys.last());
                optimizeShape(clean, polys.last());
                fitness = computeFitness(generated);

                // Update GUI
                ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                ui->polysLabel->setNum(polys.size());
                updateGuiFitness();
            }
            ui->generationLabel->setNum(generation);
            app->processEvents();
        }
    }
}

QColor Widget::optimizeColors(QImage& target, Poly& poly, bool redraw)
{
    /*
    // Find the poly's bounding box
    int minx,miny,maxx,maxy;
    minx = maxx = poly.points[0].x();
    miny = maxy = poly.points[0].y();
    for (QPoint point : poly.points)
    {
        minx = min(minx, point.x());
        maxx = max(maxx, point.x());
        miny = min(miny, point.y());
        maxy = max(maxy, point.y());
    }
    QRect box(minx, miny, maxx-minx, maxy-miny);
    */

    // Check if the pic is better, commit and return if it is
    auto validate = [&]()
    {
        QImage newGen = target;
        if (redraw)
            this->redraw(newGen);
        else
            drawPoly(newGen, poly);
        int newFit = computeFitness(newGen);
        generation++;
        ui->generationLabel->setNum(generation);
        if (newFit < fitness)
        {
            // Update data
            generated = newGen;
            fitness = newFit;

            // Update GUI
            ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            updateGuiFitness();
            return true;
        }
        else
            return false;
    };

    int targetColor;
    for (targetColor=0; targetColor <= 8; targetColor++)
    {
        do
        {
            app->processEvents();
            QColor color = poly.color;
            if (targetColor == 0)
                color = color.lighter(110); // Lighter
            else if (targetColor == 1)
                color = color.darker(110); // Darker
            else if (targetColor == 2)
                color.setRed(min(color.red()+N_COLOR_VAR,255)); // More red
            else if (targetColor == 3)
                color.setBlue(max(color.blue()-N_COLOR_VAR,0)); // Less blue
            else if (targetColor == 4)
                color.setGreen(min(color.green()+N_COLOR_VAR,255)); // More green
            else if (targetColor == 5)
                color.setRed(max(color.red()-N_COLOR_VAR,0)); // Less red
            else if (targetColor == 6)
                color.setBlue(min(color.blue()+N_COLOR_VAR,255)); // More blue
            else if (targetColor == 7)
                color.setGreen(max(color.green()-N_COLOR_VAR,0)); // Less green
            else if (targetColor == 8)
                color.setAlpha(max(color.alpha()-N_COLOR_VAR,0)); // Less alpha
            else if (targetColor == 9 && OPT_INCREASE_ALPHA)
                color.setAlpha(min(color.alpha()+N_COLOR_VAR,255)); // More alpha
            poly.color = color;
        } while (validate());
    }
    app->processEvents();
    return poly.color;
}

void Widget::optimizeShape(QImage& target, Poly& poly, bool redraw)
{
    // Check if the pic is better, commit and return if it is
    auto validate = [&]()
    {
        QImage newGen = target;
        if (redraw)
            this->redraw(newGen);
        else
            drawPoly(newGen, poly);
        int newFit = computeFitness(newGen);
        generation++;
        ui->generationLabel->setNum(generation);
        if (newFit < fitness)
        {
            // Update data
            generated = newGen;
            fitness = newFit;

            // Update GUI
            ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            updateGuiFitness();
            return true;
        }
        else
            return false;
    };

    for (QPoint& point : poly.points)
    {
        // Only try once each directions until they stop working
        // Instead of retrying other directions after one stops working
        // Call repeatedly to optimize further
        int direction;
        for (direction=0; direction<4; direction++)
        {
            do
            {
                app->processEvents();
                if (direction==0)
                    point.setY(max(point.y()-N_POS_VAR,0));
                else if (direction==1)
                    point.setX(min((unsigned)point.x()+N_POS_VAR, width));
                else if (direction==2)
                    point.setY(min((unsigned)point.y()+N_POS_VAR, height));
                else if (direction==3)
                    point.setX(max(point.x()-N_POS_VAR,0));
            } while (validate());
        }
    }
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
    poly.color.setAlpha(qrand()%180+20);
#endif
    return poly;
}

void Widget::drawPoly(QImage& target, Poly& poly)
{
    QPainter painter(&target);
    painter.setPen(QPen(Qt::NoPen));
    QBrush brush(poly.color);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawPolygon(poly.points.data(), poly.points.size());
}

void Widget::redraw(QImage& target)
{
    target.fill(Qt::white);
    for (Poly& poly : polys)
        drawPoly(target, poly);
}

void Widget::cleanDnaClicked()
{
    // Make sure we're the only one touching the polys
    setRunningGui();
    ui->btnStart->setEnabled(false);
    startStopAction->setEnabled(false);
    ui->btnStart->setText("Start");
    running = false;
    app->processEvents();

    bool ok;
    double thresholdPercent = QInputDialog::getDouble(this, "Clean DNA",
                                       "Fitness threshold", 0.0001, 0, 100, 5, &ok);
    unsigned worstFitness = width*height*3*255;
    unsigned fitnessThreshold = thresholdPercent*((double)worstFitness)/100.0;

    if (!ok)
    {
        setStoppedGui();
        startStopAction->setEnabled(true);
        ui->btnStart->setEnabled(true);
        return;
    }

    ProgressDialog progress;
    progress.setMax(polys.size());
    progress.show();

    for (int i=0; i<polys.size();)
    {
        if (!progress.isVisible())
            break;

        progress.increment();
        app->processEvents();
        // Remove broken polys
        for (QPoint& point : polys[i].points)
        {
            if (point.x() > (int)width || point.x() < 0
                || point.y() > (int)height || point.y() < 0)
            {
                polys.remove(i);
                generation++;
                ui->generationLabel->setNum(generation);
                break; // Go to the next poly
            }
        }

        // Remove polys that don't change or worsen the fitness, or are under the threshold
        QVector<Poly> polyBak = polys;
        polys.remove(i);
        redraw(generated);
        unsigned newFit = computeFitness(generated);
        if (newFit <= fitness + fitnessThreshold)
        {
            fitness = newFit;
            generation++;
            ui->generationLabel->setNum(generation);
            updateGuiFitness();
            ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            ui->polysLabel->setNum(polys.size());
            app->processEvents();
        }
        else
        {
            polys = polyBak;
            i++;
        }

    }
    setStoppedGui();
    startStopAction->setEnabled(true);
    ui->btnStart->setEnabled(true);
}

void Widget::optimizeDnaClicked()
{
    setRunningGui();
    ui->btnStart->setEnabled(false);
    startStopAction->setEnabled(false);
    ui->btnStart->setText("Start");
    running = false;

    ProgressDialog progress;
    progress.setMax(polys.size());
    progress.show();
    for (Poly& poly : polys)
    {
        if (!progress.isVisible())
            break;
        optimizeColors(generated, poly, true);
        optimizeShape(generated, poly, true);
        progress.increment();
        app->processEvents();
    }

    setStoppedGui();
    startStopAction->setEnabled(true);
    ui->btnStart->setEnabled(true);
}
