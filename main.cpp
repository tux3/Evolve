#include "widget.h"
#include <QApplication>

QApplication* app;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Evolve");
    QCoreApplication::setApplicationVersion("1.2");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("main","Polygon image evolver"));
    parser.addPositionalArgument("filename", QCoreApplication::translate("main", "Image file to imitate."));
    parser.addHelpOption();
    parser.addVersionOption();
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
 */
