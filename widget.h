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
    static quint64 computeFitness(const QImage& target);
    static QRect computeAutofocusFitness(const QImage& target);
    Poly genPoly(); // Creates a new random polygon
    QImage predraw(int polyIndex);
    static void redraw(QImage& target, QVector<Poly>& polyList = polys);
    void optimizeColors(int polyIndex, QVector<Poly>& newPolys=polys); // Will update generated directly
    void optimizeColors(int polyIndex, QVector<Poly> &newPolys, QImage& predrawn); // Will update generated directly
    void optimizeShape(int polyIndex, QVector<Poly> &newPolys=polys); // Will update generated directly
    void optimizeShape(int polyIndex, QVector<Poly>& newPolys, QImage& predrawn); // Will update generated directly
    void updateGuiFitness();
    void run(); // Run the vectorizer's main loop
    void setRunningGui();
    void setStoppedGui();
    bool virtual eventFilter(QObject *object, QEvent *event);
    // Mutations
    void tryAddPoly(); // Will gen one poly and add it to generated only if it improves fitness
    void removePoly(QVector<Poly>&, QImage& target); // Will remove a poly, without testing if it improves fitness
    void reorderPoly(QVector<Poly>&, QImage& target); // Will change the zorder of a poly, without testing if it improves fitness
    void shapeOptPoly(QVector<Poly>& newPolys);
    void movePoint(QVector<Poly>&, QImage& target);
    static void setAutofocus(bool enabled);

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
    void redrawClicked();
    void autofocusClicked();
    
private:
    Ui::Widget *ui;
    QMenuBar *menuBar;
    QAction* startStopAction, *openAction, *cleanAction, *optimizeAction;
    static QImage pic;
    QImage generated;
    bool running;
    static QVector<Poly> polys;
    static unsigned height, width;
    quint64 fitness;
    int generation;
    static QTimer autofocusTimer;
};

#endif // WIDGET_H
