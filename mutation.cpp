#include "widget.h"
#include "ui_widget.h"

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
        optimizeColors(polyPos, predrawn);
        optimizeShape(polyPos, predrawn);
        fitness = computeFitness(generated);

        // Update GUI
        ui->imgBest->setPixmap(QPixmap::fromImage(generated));
        ui->polysLabel->setNum(polyPos+1);
        updateGuiFitness();
    }
}

void Widget::removePoly(QVector<Poly>& newPolys, QImage &target)
{
    int index = qrand()%newPolys.size();
    newPolys.remove(index);
    redraw(target, newPolys);
}

void Widget::reorderPoly(QVector<Poly>& newPolys, QImage &target)
{
    int source = qrand()%newPolys.size();
    int dest = qrand()%newPolys.size();
    Poly poly = newPolys.takeAt(source);
    newPolys.insert(dest, poly);
    redraw(target, newPolys);
}
