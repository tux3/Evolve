#include "widget.h"
#include "ui_widget.h"
#include "settings.h"
#include <QDebug>

void Widget::tryAddPoly()
{
    Poly poly = genPoly();
    QImage newGen = generated;
    Poly::drawPoly(newGen, poly);
    quint64 newFit = computeFitness(newGen);
    if (newFit < fitness)
    {
        // Update data
        int polyPos = polys.size();
        polys.append(poly);
        QImage predrawn = generated;
        generated = newGen;

        // Optimize
        optimizeColors(polyPos, polys, predrawn);
        optimizeShape(polyPos, polys, predrawn);
        fitness = computeFitness(generated);

        // Update GUI
        ui->imgBest->setPixmap(QPixmap::fromImage(generated));
        ui->polysLabel->setNum(polyPos+1);
        updateGuiFitness();
    }
}

void Widget::removePoly(QVector<Poly>& newPolys, QImage &target)
{
    int w = (int)(((float)width*(float)(FOCUS_RIGHT-FOCUS_LEFT))/100.0);
    int h = (int)(((float)height*(float)(FOCUS_BOTTOM-FOCUS_TOP))/100.0);
    int x = (width*(float)FOCUS_LEFT/100.0);
    int y = (height*(float)FOCUS_TOP/100.0);
    int index;
    for (int i=0;i<100;i++)
    {
        index = qrand()%newPolys.size();
        if (newPolys[index].hasPointIn(QRect(x,y,w,h)))
            break;
    }
    newPolys.remove(index);
    redraw(target, newPolys);
}

void Widget::shapeOptPoly(QVector<Poly>& newPolys)
{
    int index = qrand()%newPolys.size();
    optimizeShape(index, newPolys);
}

void Widget::reorderPoly(QVector<Poly>& newPolys, QImage &target)
{
    int w = (int)(((float)width*(float)(FOCUS_RIGHT-FOCUS_LEFT))/100.0);
    int h = (int)(((float)height*(float)(FOCUS_BOTTOM-FOCUS_TOP))/100.0);
    int x = (width*(float)FOCUS_LEFT/100.0);
    int y = (height*(float)FOCUS_TOP/100.0);
    int source;
    for (int i=0;i<100;i++)
    {
        source = qrand()%newPolys.size();
        if (newPolys[source].hasPointIn(QRect(x,y,w,h)))
            break;
    }
    int dest = qrand()%newPolys.size();
    Poly poly = newPolys.takeAt(source);
    newPolys.insert(dest, poly);
    redraw(target, newPolys);
    optimizeShape(dest, newPolys);
    optimizeColors(dest, newPolys);
}

void Widget::movePoint(QVector<Poly>& newPolys, QImage& target)
{
    int w = (int)(((float)width*(float)(FOCUS_RIGHT-FOCUS_LEFT))/100.0);
    int h = (int)(((float)height*(float)(FOCUS_BOTTOM-FOCUS_TOP))/100.0);
    int x = (width*(float)FOCUS_LEFT/100.0);
    int y = (height*(float)FOCUS_TOP/100.0);
    int source;
    for (int i=0;i<100;i++)
    {
        source = qrand()%newPolys.size();
        if (newPolys[source].hasPointIn(QRect(x,y,w,h)))
            break;
    }
    Poly poly = newPolys[source];
    int pointi = qrand()%poly.points.size();
    poly.points[pointi].setX(poly.points[pointi].x()+qrand()%20-10);
    poly.points[pointi].setY(poly.points[pointi].y()+qrand()%20-10);
    redraw(target, newPolys);
    optimizeShape(source, newPolys);
    optimizeColors(source, newPolys);
}
