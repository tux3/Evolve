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
    fileMenu->addAction("&Open image", this, SLOT(openImageClicked()));
    fileMenu->addAction("&Save image", this, SLOT(saveImageClicked()));
    fileMenu->addAction("&Export as SVG", this, SLOT(saveSVGClicked()));
    fileMenu->addSeparator();
    startStopAction = fileMenu->addAction("S&tart");
    fileMenu->addAction("&Quit", this, SLOT(close()));
    QMenu* dnaMenu = menuBar->addMenu(tr("&DNA"));
    dnaMenu->addAction("&Import DNA", this, SLOT(importDnaClicked()));
    dnaMenu->addAction("&Export DNA", this, SLOT(exportDnaClicked()));
    dnaMenu->addAction("&Clean DNA", this, SLOT(cleanDnaClicked()));
    dnaMenu->addAction("&Optimize DNA", this, SLOT(optimizeDnaClicked()));
    QMenu* settingsMenu = menuBar->addMenu(tr("&Settings"));
    settingsMenu->addAction("&Settings", this, SLOT(settingsClicked()));
    ui->gridLayout->addWidget(menuBar,0,0,1,4);
    menuBar->setFixedHeight(22);

    generation = 0;
    running = false;

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openImageClicked()));
    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(startClicked()));

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
        for (unsigned i=0; i<end-start; i++)
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
    for (int i=0; i < N_CORES; i++){
        slices[i].waitForFinished();
    }
    return fitness.load();
}

void Widget::run()
{
    int newFit;
    // Main loop
    while (running /* && polys.size() < N_POLYS */ )
    {
        Poly poly;
        QImage newGen = generated;
        if (polys.length() >= 30){
            int i = qrand() % polys.length();
            poly = polys[i];
            int count = polys[i].points.length();
            int miny=999, maxy=0, minx=999,maxx=0;
            for(int j=0;j<count;j++){
                int curx=polys[i].points[j].x();
                int cury=polys[i].points[j].y();
                maxx = max(curx,maxx);
                maxy = max(cury,maxy);
                minx = min(curx,minx);
                miny = min(cury,miny);
                polys[i].points[j].setX(qrand() % width);
                polys[i].points[j].setY(qrand() % height);
            }
            redraw(newGen);
            //QRect q (minx,miny,maxx,maxy);
            newFit = computeFitness(newGen);
            //int oldFit = computeFitness(generated,q);
            if (newFit >= fitness) { //why does it keep swapping itself?
                polys[i]=poly; //undo
            } else {
                QImage clean = generated;
                optimizeColors(clean, poly);
                fitness = computeFitness(generated);
                ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                updateGuiFitness();

            }
        }
        newGen = generated;
        poly = genPoly();
        drawPoly(newGen, poly);
        generation++;
        newFit = computeFitness(newGen);
        if (newFit < fitness)
        {

            polys.append(poly);

            // Optimize colors
            QImage clean = generated;
            optimizeColors(clean, polys.last());

            // Update data
            //generated = newGen;
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

QColor Widget::optimizeColors(QImage& target, Poly& poly, bool redraw)
{
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

    int targetColor; // 0=R, 1=G, 2=B, 3=A

    // Add
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

Poly Widget::genPoly()
{
    Poly poly;
    for (int i=0; i<N_POLY_POINTS; i++)
    {
        quint16 x,y;
        x = qrand()%(int)(width*(((float)FOCUS_RIGHT-FOCUS_LEFT)/100)) + (width*(float)FOCUS_LEFT/100);
        y = qrand()%(int)(height*(((float)FOCUS_BOTTOM-FOCUS_TOP)/100)) + (height*(float)FOCUS_TOP/100);
        poly.points.append(QPoint(x,y));
    }
#if GEN_WITH_RANDOM_COLOR
    poly.color = QColor::fromRgb(qrand()*qrand()*qrand());
    poly.color.setAlpha(qrand()%130+20);
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
    poly.color.setAlpha(qrand()%130+20);
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
    ui->btnStart->setEnabled(false);
    startStopAction->setEnabled(false);
    ui->btnStart->setText("Start");
    running = false;
    app->processEvents();

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

                // Go to the next poly
                break;
            }
        }

        // Remove polys that don't change or worsen the fitness
        QVector<Poly> polyBak = polys;
        polys.remove(i);
        redraw(generated);
        int newFit = computeFitness(generated);
        if (newFit <= fitness)
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
    startStopAction->setEnabled(true);
    ui->btnStart->setEnabled(true);
}

void Widget::optimizeDnaClicked()
{
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
        progress.increment();
        app->processEvents();
    }

    startStopAction->setEnabled(true);
    ui->btnStart->setEnabled(true);
}
