#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QVector>
#include <QMenuBar>
#include <stdint.h>

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
    static int computeFitness(const QImage& target, const QRect box=QRect());
    Poly genPoly(); // Creates a new random polygon
    static void drawPoly(QImage& target, Poly& poly);
    static void drawPoly(QImage& target, Poly& poly, QPainter& painter);
    void redraw(QImage& target);
    QColor optimizeColors(QImage& target, Poly& poly, bool redraw=false);
    void optimizeShape(QImage& target, Poly& poly, bool redraw=false);
    void updateGuiFitness();
    void run(); // Run the vectorizer's main loop
    void setRunningGui();
    void setStoppedGui();
    bool virtual eventFilter(QObject *object, QEvent *event);

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
    void githubClicked();
    void focusClicked();
    
private:
    Ui::Widget *ui;
    QMenuBar *menuBar;
    QAction* startStopAction, *openAction, *cleanAction, *optimizeAction;
    static QImage pic;
    QImage generated;
    bool running;
    QVector<Poly> polys;
    static unsigned height, width;
    int fitness, generation;
};

#endif // WIDGET_H
