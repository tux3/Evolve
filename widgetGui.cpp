#include "widget.h"
#include "ui_widget.h"
#include "settingswidget.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFile>
#include <QtSvg/QSvgGenerator>
#include <QPainter>

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
        ui->btnStart->setText("Start");
        startStopAction->setText("S&tart");
        return;
    }
    if (pic.isNull())
        return;
    running=true;
    ui->btnStart->setText("Stop");
    startStopAction->setText("S&top");

    run();
}
