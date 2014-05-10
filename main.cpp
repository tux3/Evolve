#include "widget.h"
#include <QApplication>
#include <QCommandLineParser>

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
    /*qDebug() << "filename:  " <<  QString filename;
    QCommandLineOption accuracyOption(QStringList() << "n" << "accuracy", QCoreApplication::translate("main", "Evolve until this accuracy is reached."));
    parser.addOption(noListOption);
    QCommandLineOption outputFileOption(QStringList() << "n" << "output-file", QCoreApplication::translate("main", "Specify output file."));
    parser.addOption(outputFileOption);
    QCommandLineOption cliOnlyOption(QStringList() << "c" << "cli-only", QCoreApplication::translate("main", "Don't display gui."));
    parser.addOption(cliOnlyOption);
    */
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
 * Okay, we have color optimizations, now we just need to add shape optimization just after the color.
 * It works like this : You find a suitable place to add a new poly, correct the color to fit the new place's color perfectly
 * and then reshape the poly to cover that new place perfectly. Or at least cover it as much as we can do reasonably fast.
 *
 */
