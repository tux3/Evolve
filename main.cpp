#include "widget.h"
#include <QApplication>
//#include <QCommandLineParser>

QApplication* app;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Evolve");
    QCoreApplication::setApplicationVersion("1.2");
    /*
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
 * We need bounding boxes when optimizing a particular poly
 * Maybe we could not process the whole image's fitness until after we're done optimizing
 * We just need to know if the bounding box's zone improved
 * This should give a huge boost, especially for smaller polys.
 * We just need to be careful in optimizeShape since the bounding box's dimensions change every iteration
 * Most everywhere else we can just compute the bounding box once at the start.
 *
 * Apply optimizeShape's redraw optimizations to optimizeColor
 *
 * For computeFitness precompute the original.
 * When we load the original pic, immediatly create 3 arrays for the R, G, and B of each pixel.
 * It's probably faster to use one hueg contiguous array rather than a multidimentional, for each color.
 * We'll just use i*width+j with i*width computed out of the inner loop.
 *
 * Maybe in computeFitness we could convert everything to RGB instead of RGBA, so we'd always have 0xFF for the A
 * And then blindly compute our difference of uint8_t, this way we might get vector instructions and unrolling.
 *
 * Stuff from EvoLisa :
 * Can add new polygons at a random zindex instead of always on top
 * Insert a new point in the middle of two points (then we could optimize the new point)
 * Remove a point
 * Can do multiple mutations at the same time. Can do no mutation at all.
 * Maybe try to do this too, but we should avoid empty mutation loops if possible.
 * Make it likely that we'll always have at least one mutation every loop.
 * His probabilities of adding/removing points are per poly, so when you have a lot of polys
 * the polys mutation loop is the only thing running, and it almost never adds/removes/moves polys, just mutate.
 * And each poly mutated also has a point mutation loop. So it'd end up taking all the damn time with our polys.
 * We should make it per mutation loop instead. Since we can have up to 10k polys without problem it seems better.
 * He has separate mutation rates for everything.
 * He has a min/max poly count and min/max point count.
 * When mutating, accept <= changes in fitness. We accept +fitnessThreshold< currently
 * Maybe keep the +fitnessThreshold< for adding the initial polys (until we reach minPolys)
 * Then start mutating using <=. We shouldn't be able to get stuck doing/undoing in a loop,
 * since the mutation process is mostly random, not optimizations.
 * When we add a completely new poly, we'll still want to optimize its color/shape.
 * But in the normal mutations we don't optimize the shape/color of random polys.
 *
 */
