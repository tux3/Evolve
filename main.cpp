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
    QCommandLineOption accuracyOption(QStringList() << "n" << "accuracy", QCoreApplication::translate("main", "Evolve until this accuracy is reached."));
    parser.addOption(accuracyOption);
    QCommandLineOption outputFileOption(QStringList() << "n" << "output-file", QCoreApplication::translate("main", "Specify output file."));
    parser.addOption(outputFileOption);
    QCommandLineOption cliOnlyOption(QStringList() << "c" << "cli-only", QCoreApplication::translate("main", "Don't display gui."));
    parser.addOption(cliOnlyOption);
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
 * After a limit of poly is reached (set in settings) maybe add a virtual fitness penalty to new polys
 * Prefer modifying existing polys instead.
 *
 * We need command line control. For science.
 *
 * Maybe the idea of autofocus isn't that bad.
 * Compute the fitness for X*X chunks (grid of size X).
 * Then compute the fitness for the overlapped (X-1)*(X-1) chunks
 * Then compute the average fitness.
 * Then focus on each chunks for at least 1000 generations and at least until they all reach the average fitness.
 * When we've cycled throught each chunk, recompute the fitnesses and cycle again.
 *
 * Here's a silly idea, when we add polys, we could sort them in 5 piles :
 * Covers the whole image, or covers only one of the 4 corner.
 * Then when redrawing the full image, we can separate into 4 threads.
 * They each draw their own pile + the common pile, and at the end you stitch the 4 drawn corner in the full pic.
 * This would work well with the autofocus.
 *
 * Profiling says the really expensive part is the QColor ctor and getRgb
 * So basically the inner loop of computeFitness
 *
 * Optimization : When we need to redraw several times for a poly, only draw the previous polys once
 * Keep the image with the previous polys, modify the poly, and only redraw the ones on top every time.
 *
 * In optimizeShape and also a little in optimiseColor (but way less), we spend too much time processing events.
 * Maybe only process 1/3 of the time, or something. Make this configurable ?
 *
 * We still spend most of our time in computeFitness.
 * A working computeFitness with bounding boxes would be a huge improvement.
 * Maybe we could not process the whole image's fitness until after we're done optimizing
 * We just need to know if the bounding box's zone improved.
 *
 */
