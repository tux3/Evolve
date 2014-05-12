#include "widget.h"
#include "ui_widget.h"
#include "settingswidget.h"
#include "settings.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFile>
#include <QtSvg/QSvgGenerator>
#include <QPainter>
#include <QThreadPool>
#include <QDebug>
#include <QMouseEvent>

using namespace std;

void Widget::closeEvent(QCloseEvent *event)
{
    running = false;
    event->accept();
    exit(0);
}

void Widget::openImageClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Open Image","", "Images (*.png *.gif *.jpg *.jpeg *.bmp)");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    running = false;

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

    FOCUS_LEFT=0;
    FOCUS_RIGHT=100;
    FOCUS_TOP=0;
    FOCUS_BOTTOM=100;

    generation = 0;
    ui->generationLabel->setNum(0);
    fitness = computeFitness(generated);
    updateGuiFitness();
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
    updateGuiFitness();
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

void Widget::settingsClicked()
{
    SettingsWidget settingsWidget;
    settingsWidget.show();
    while (settingsWidget.isVisible())
        app->processEvents();
}

void Widget::updateGuiFitness()
{
    int worstFitness = width*height*3*255;
    float percentFitness = 100.0-((double)fitness/(double)worstFitness*100.0);
    ui->fitnessLabel->setNum(percentFitness);
    ui->fitnessLabel->setText(ui->fitnessLabel->text()+'%');
}


void Widget::startClicked()
{
    if (running)
    {
        running = false;
        setStoppedGui();
        return;
    }
    if (pic.isNull())
        return;
    running=true;
    setRunningGui();

    run();
}

void Widget::githubClicked()
{
#ifdef WIN32
    system("start http://github.com/tux3/Evolve");
#else
    QMessageBox::information(this,"GitHub","http://github.com/tux3/Evolve");
#endif
}

void Widget::setRunningGui()
{
    ui->btnOpen->setEnabled(false);
    openAction->setEnabled(false);
    cleanAction->setEnabled(false);
    optimizeAction->setEnabled(false);
    startStopAction->setText("S&top");
    ui->btnStart->setText("Stop");
}

void Widget::setStoppedGui()
{
    startStopAction->setText("S&tart");
    ui->btnStart->setText("Start");
    ui->btnOpen->setEnabled(true);
    openAction->setEnabled(true);
    cleanAction->setEnabled(true);
    optimizeAction->setEnabled(true);
}

void Widget::focusClicked()
{
    QMessageBox::information(this, "Focus",
    "Draw a rectangle on the original image to focus on it.\nRight click to cancel.");
}

bool Widget::eventFilter(QObject *object, QEvent *event)
{
    static bool pressed = false; // Is the left mouse button pressed
    static QPoint startCoords;
    if (object == ui->imgOriginal && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            //QMessageBox::information(this, "Focus", "Clicked");
            pressed = true;
            startCoords = mouseEvent->pos();
        }
        else if (mouseEvent->button() == Qt::RightButton)
        {
            ui->imgOriginal->setPixmap(QPixmap::fromImage(pic));
            FOCUS_LEFT = FOCUS_TOP = 0;
            FOCUS_RIGHT = FOCUS_BOTTOM = 100;
        }
    }
    else if (object == ui->imgOriginal && event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton)
            pressed = false;
    }
    else if (pressed && object == ui->imgOriginal && event->type() == QEvent::MouseMove)
    {
        // Draw the focus rect
        QPixmap newpic = QPixmap::fromImage(pic);
        //newpic = newpic.scaled(ui->imgOriginal->size());
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint endCoords = mouseEvent->pos();
        endCoords.setX(max(min(endCoords.x(), ui->imgOriginal->width()),0));
        endCoords.setY(max(min(endCoords.y(), ui->imgOriginal->height()),0));
        QPoint scaledStart = startCoords;
        scaledStart.rx() *= (float)width/ui->imgBest->width();
        scaledStart.ry() *= (float)height/ui->imgBest->height();
        QPoint scaledEnd = endCoords;
        scaledEnd.rx() *= (float)width/ui->imgBest->width();
        scaledEnd.ry() *= (float)height/ui->imgBest->height();
        QPainter paint;
        paint.begin(&newpic);
        QPen pen(Qt::SolidLine);
        pen.setWidth(3*max((float)width/ui->imgBest->width(), (float)height/ui->imgBest->height()));
        pen.setColor(QColor(200,0,0,150));
        paint.setPen(pen);
        paint.drawRect(QRect(scaledStart, scaledEnd));
        paint.end();
        ui->imgOriginal->setPixmap(newpic);

        // Set the focus coords
        FOCUS_LEFT = min(scaledStart.x(),scaledEnd.x())*100/newpic.width();
        FOCUS_RIGHT = max(scaledStart.x(),scaledEnd.x())*100/newpic.width();
        FOCUS_TOP = min(scaledStart.y(),scaledEnd.y())*100/newpic.height();
        FOCUS_BOTTOM = max(scaledStart.y(),scaledEnd.y())*100/newpic.height();
        if (FOCUS_LEFT == 100)
            FOCUS_LEFT = 99;
        if (FOCUS_TOP == 100)
            FOCUS_TOP = 99;
        if (FOCUS_RIGHT <= FOCUS_LEFT)
            FOCUS_RIGHT = FOCUS_LEFT+1;
        if (FOCUS_BOTTOM <= FOCUS_TOP)
            FOCUS_BOTTOM = FOCUS_TOP+1;
    }
    return false;
}
