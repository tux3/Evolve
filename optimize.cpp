#include "widget.h"
#include "ui_widget.h"
#include "settings.h"
#include <QPainter>
#include <QPen>
#include <QDebug>

using namespace std;

void Widget::optimizeColors(int polyIndex, QVector<Poly>& newPolys)
{
    QImage predrawn = predraw(polyIndex);
    optimizeColors(polyIndex, newPolys, predrawn);
}

void Widget::optimizeColors(int polyIndex, QVector<Poly>& newPolys, QImage& predrawn)
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

    int polysSize = newPolys.size();
    Poly& poly = newPolys[polyIndex];
    static QBrush brush(Qt::SolidPattern);
    int processEventsRatelimit = 0;

    // Check if the pic is better, commit and return if it is
    auto validate = [&]()
    {
        QImage newGen = predrawn;
        QPainter painter(&newGen);
        painter.setPen(QPen(Qt::NoPen));
        for (int i=polyIndex; i<polysSize; ++i)
        {
            brush.setColor(newPolys[i].color);
            painter.setBrush(brush);
            painter.drawPolygon(newPolys[i].points.data(), newPolys[i].points.size());
        }
        quint64 newFit = computeFitness(newGen);
        generation++;
        ui->generationLabel->setNum(generation);
        if (newFit < fitness)
        {
            // Update data
            generated = newGen;
            fitness = newFit;

            // Update GUI
            if (processEventsRatelimit==0)
            {
                ui->imgBest->setPixmap(QPixmap::fromImage(generated));
                updateGuiFitness();
            }
            return true;
        }
        else
            return false;
    };

    app->processEvents();
    int targetColor;
    for (targetColor=0; targetColor <= 8; targetColor++)
    {
        for(;;)
        {
            if (processEventsRatelimit == GUI_REFRESH_RATE) // processEvents is a massive slowdown
            {
                processEventsRatelimit=0;
                app->processEvents();
            }
            else
                processEventsRatelimit++;
            QColor color = poly.color, oldcolor=poly.color;
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
            if (!validate())
            {
                poly.color = oldcolor;
                break;
            }
        }
    }
    generated = predrawn;
    Poly::drawPoly(generated, poly);
    app->processEvents();
}

void Widget::optimizeShape(int polyIndex, QVector<Poly>& newPolys)
{
    QImage predrawn = predraw(polyIndex);
    optimizeShape(polyIndex, newPolys, predrawn);
}

void Widget::optimizeShape(int polyIndex, QVector<Poly>& newPolys, QImage& predrawn)
{
    int polysSize = newPolys.size();
    Poly& poly = newPolys[polyIndex];
    static QBrush brush(Qt::SolidPattern);

    // Check if the pic is better, commit and return if it is
    auto validate = [&]()
    {
        QImage newGen = predrawn;
        QPainter painter(&newGen);
        painter.setPen(QPen(Qt::NoPen));
        for (int i=polyIndex; i<polysSize; ++i)
        {
            brush.setColor(newPolys[i].color);
            painter.setBrush(brush);
            painter.drawPolygon(newPolys[i].points.data(), newPolys[i].points.size());
        }
        quint64 newFit = computeFitness(newGen);
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

    int processEventsRatelimit = 0;
    for (QPoint& point : poly.points)
    {
        // Only try once each directions until they stop working
        // Instead of retrying other directions after one stops working
        // Call repeatedly to optimize further
        int direction;
        for (direction=0; direction<4; direction++)
        {
            for(;;)
            {
                if (processEventsRatelimit == GUI_REFRESH_RATE) // processEvents is a massive slowdown
                {
                    processEventsRatelimit=0;
                    app->processEvents();
                }
                else
                    processEventsRatelimit++;

                if (direction==0)
                {
                    int oldy = point.y();
                    point.setY(max(oldy-N_POS_VAR,0));
                    if (!validate())
                    {
                        point.setY(oldy);
                        break;
                    }
                    else if (point.y()==0)
                        break;
                }
                else if (direction==1)
                {
                    int oldx = point.x();
                    point.setX(min((unsigned)oldx+N_POS_VAR, width));
                    if (!validate())
                    {
                        point.setX(oldx);
                        break;
                    }
                    else if (point.x()==(int)width)
                        break;
                }
                else if (direction==2)
                {
                    int oldy = point.y();
                    point.setY(min((unsigned)oldy+N_POS_VAR, height));
                    if (!validate())
                    {
                        point.setY(oldy);
                        break;
                    }
                    else if (point.y()==(int)height)
                        break;
                }
                else if (direction==3)
                {
                    int oldx = point.x();
                    point.setX(max(oldx-N_POS_VAR,0));
                    if (!validate())
                    {
                        point.setX(oldx);
                        break;
                    }
                    else if (point.x()==0)
                        break;
                }
            }
        }
    }
    generated = predrawn;
    Poly::drawPoly(generated, poly);
    app->processEvents();
}
