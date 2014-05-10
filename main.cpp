#include "widget.h"
#include <QApplication>

QApplication* app;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    app = &a;
    Widget w;
    w.show();
    
    return a.exec();
}

/**
 *
 * TODO
 * Make the DNA loading size independent. If it's not the same size, then resize the polys. It's a vector.
 *
 * Make an option to only redraw around a particular poly, in a bounding box around this poly
 *
 * The crash might be because we write to an image while it's trying to render it
 * Careful with writing to variables from other threads
 *
 */
