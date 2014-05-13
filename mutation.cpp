#include "widget.h"
#include "ui_widget.h"

void Widget::tryAddPoly()
{
    Poly poly = genPoly();
    QImage newGen = generated;
    drawPoly(newGen, poly);
    int newFit = computeFitness(newGen);
    if (newFit < fitness)
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
}
