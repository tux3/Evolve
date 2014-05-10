#include "widget.h"
#include "ui_widget.h"
#include "progressdialog.h"
#include <QCloseEvent>
#include <QDataStream>
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QPixmap>
#include <QPen>
#include <QPainter>
#include <QMessageBox>
#include <QRgb>
#include <QtSvg/QSvgGenerator>
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

void Widget::closeEvent(QCloseEvent *event)
{
    running = false;
    event->accept();
    exit(0);
}

void Widget::openImageClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Open Image","", "Images (*.png *.jpg *.jpeg *.bmp)");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    pic.loadFromData(file.readAll());
    pic = pic.convertToFormat(QImage::Format_ARGB32);
    ui->imgOriginal->setPixmap(QPixmap::fromImage(pic));

    // Update our data
    height = pic.height();
    width = pic.width();

    // Create a blank pic the size of the original
    generated = QImage(pic.width(), pic.height(), QImage::Format_ARGB32);
    generated.fill(QColor(255,255,255));
    generated = generated.convertToFormat(QImage::Format_ARGB32);
    ui->imgBest->setPixmap(QPixmap::fromImage(generated));

    fitness = computeFitness(generated);
    ui->fitnessLabel->setNum(fitness);
    polys.clear();
    file.close();
}

void Widget::saveImageClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"),"",
                                                    "Images (*.png *.jpg)");
    if (fileName.isEmpty())
        return;
    generated.save(fileName);
}

void Widget::importDnaClicked()
{
    unsigned dnaHeight, dnaWidth;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import DNA"),"",
                                                    "DNA (*.dna)");
    if (fileName.isEmpty())
        return;
    polys.clear();
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_0);
    in >> generation;
    in >> dnaWidth;
    in >> dnaHeight;
    in >> polys;
    file.close();

    if (dnaWidth != width || dnaHeight != height)
    {
        QMessageBox::critical(this,"Invalid image","The original image and the DNA have different dimentions");
        return;
    }

    redraw(generated);
    fitness  = computeFitness(generated);
    ui->fitnessLabel->setNum(fitness);
    ui->generationLabel->setNum(generation);
    ui->polysLabel->setNum(polys.size());
    ui->imgBest->setPixmap(QPixmap::fromImage(generated));
}

void Widget::exportDnaClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export DNA"),"",
                                                    "DNA (*.dna)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_0);
    out << generation;
    out << width;
    out << height;
    out << polys;
    file.close();
}

void Widget::saveSVGClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export SVG"),"",
                                                    "SVG (*.svg)");
    if (fileName.isEmpty())
        return;
    QSvgGenerator generator;
    generator.setFileName(fileName);
    //generator.setSize(QSize(width, height));
    generator.setTitle("Mlkj's Bruteforce Vectorizer");

    QPainter painter;
    painter.begin(&generator);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setPen(QPen(Qt::NoPen));

    painter.fillRect(0,0,width,height,Qt::white);
    for (Poly& poly : polys)
    {
        QBrush brush(poly.color);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);
        painter.drawPolygon(poly.points.data(), poly.points.size());
    }
    painter.end();
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

    auto computeSlice = [&](unsigned start, unsigned end)
    {
        QRgb* targetLine;
        QRgb* originalLine;
        for (unsigned i=start; i<end; i++)
        {
            targetLine = (QRgb*)target.scanLine(i);
            originalLine = (QRgb*)pic.scanLine(i);
            // Sum of the differences of each pixel's color
            for (unsigned j=minx; j<maxx; j++)
            {
                int tR,tG,tB;
                int oR,oG,oB;
                QColor(targetLine[j]).getRgb(&tR,&tG,&tB);
                QColor(originalLine[j]).getRgb(&oR,&oG,&oB);
                unsigned diff = abs(tR-oR)+abs(tG-oG)+abs(tB-oB);
                fitness.fetchAndAddRelaxed(diff);
            }
        }
    };
    QFuture<void> firstSlice = QtConcurrent::run(computeSlice, miny, maxy/2);
    computeSlice(maxy/2, maxy);
    firstSlice.waitForFinished();

    return fitness.load();
}

void Widget::startClicked()
{
    if (running)
    {
        running = false;
        ui->btnStart->setText("Start");
        startStopAction->setText("S&tart");
        return;
    }
    if (pic.isNull())
        return;
    running=true;
    ui->btnStart->setText("Stop");
    startStopAction->setText("S&top");

    // Gen some initial polys if the pic is empty
    while (polys.size() < 5)
    {
        Poly poly = genPoly();
        polys.append(poly);
        QImage clean = generated;
        optimizeColors(clean, polys.last());
    }

    // Main loop
    Poly poly;
    QImage newGen;
    QImage clean;
    while (running /* && polys.size() < N_POLYS */ )
    {
        poly = genPoly();
        newGen = generated;
        drawPoly(newGen, poly);
        generation++;
        int newFit = computeFitness(newGen);
        if (newFit < fitness)
        {
            polys.append(poly);

            // Optimize colors
            clean = generated;
            optimizeColors(clean, polys.last());

            // Update data
            //generated = newGen;
            fitness = computeFitness(generated);

            // Update GUI
            ui->imgBest->setPixmap(QPixmap::fromImage(generated));
            ui->polysLabel->setNum(polys.size());
            ui->fitnessLabel->setNum(fitness);
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
            ui->fitnessLabel->setNum(fitness);
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
            else if (targetColor == 9)
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
    poly.color = QColor::fromRgb(qrand()*qrand()*qrand());
    poly.color.setAlpha(qrand()%100);
    for (int i=0; i<N_POLY_POINTS; i++)
    {
        quint16 x,y;
        x = qrand()%width;
        y = qrand()%height;
        poly.points.append(QPoint(x,y));
    }
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
    ui->btnStart->setText("Start");
    running = false;
    app->processEvents();

    ProgressDialog progress;
    progress.setMax(polys.size());
    progress.show();

    for (int i=0; i<polys.size();)
    {
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
            ui->fitnessLabel->setNum(fitness);
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
    ui->btnStart->setEnabled(true);
}

void Widget::optimizeDnaClicked()
{
    ProgressDialog progress;
    progress.setMax(polys.size());
    progress.show();
    for (Poly& poly : polys)
    {
        optimizeColors(generated, poly, true);
        progress.increment();
        app->processEvents();
    }
}

void Widget::settingsClicked()
{

}
