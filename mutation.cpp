#include "widget.h"
#include "ui_widget.h"

void Widget::tryAddPoly()
{
    Poly poly = genPoly();
    QImage newGen = generated;
    Poly::drawPoly(newGen, poly);
    int newFit = computeFitness(newGen);
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

void Widget::removePoly(QImage &target)
{
    // TODO !
}

void Widget::reorderPoly(QImage& target)
{
    // TODO !
}
