#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QVector>
#include <QMenuBar>

#include "poly.h"

// Number of points of a polygon
#define N_POLY_POINTS 3
// Variation of color when optimizing. Lower if slower but better
#define N_COLOR_VAR 5

extern QApplication* app;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget();
    int computeFitness(QImage& target, QRect box=QRect());
    static Poly genPoly(); // Creates a new random polygon
    static void drawPoly(QImage& target, Poly& poly);
    void redraw(QImage& target);
    QColor optimizeColors(QImage& target, Poly& poly, bool redraw=false);
    ~Widget();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void openImageClicked();
    void saveImageClicked();
    void saveSVGClicked();
    void importDnaClicked();
    void exportDnaClicked();
    void cleanDnaClicked();
    void optimizeDnaClicked();
    void startClicked();
    void settingsClicked();
    
private:
    Ui::Widget *ui;
    QMenuBar *menuBar;
    QAction* startStopAction;
    QImage pic;
    QImage generated;
    bool running;
    QVector<Poly> polys;
    static unsigned height, width;
    int fitness, generation;
};

#endif // WIDGET_H
