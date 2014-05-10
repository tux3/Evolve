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
 * Anon gets a crash when changing the vertex/numpoints count while running.
 *
 * We have a crash in computeFitness, at targetLine[j].getRgb
 * Using a focus on top right corner
 * Might be due to the threading. QImage is not thread safe (just reentrant)
 * Yep, probably. Can't reproduce withou the threading.
 *
 * Crash on clicking start on Debian
 * "Illegal instruction". What the fuck ?
 *
 * After a limit of poly is reached (set in settings) maybe add a virtual fitness penalty to new polys
 * Prefer modifying existing polys instead.
 *
 * We need command line control. For science.
 *
 * Maybe the idea of autofocus isn't that bad. Compute the fitness for 100 chunks, and focus on each. Then focus on the borders to fix them
 *
 */
