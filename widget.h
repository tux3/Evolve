#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QVector>
#include <QMenuBar>

#include "poly.h"


extern QApplication* app;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget();
    ~Widget();
    int computeFitness(QImage& target, QRect box=QRect());
    Poly genPoly(); // Creates a new random polygon
    static void drawPoly(QImage& target, Poly& poly);
    void redraw(QImage& target);
    QColor optimizeColors(QImage& target, Poly& poly, bool redraw=false);
    void updateGuiFitness();
    void run(); // Run the vectorizer's main loop

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
