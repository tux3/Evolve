#include "settings.h"

unsigned N_POLY_POINTS = 4;
unsigned N_COLOR_VAR = 10;
unsigned N_POS_VAR = 0;
unsigned GUI_REFRESH_RATE = 1;
unsigned AUTOFOCUS_RES = 2;
unsigned N_CORES = 4;

unsigned FOCUS_LEFT=0, FOCUS_RIGHT=100, FOCUS_TOP=0, FOCUS_BOTTOM=100;

unsigned POLYS_MIN=15, POLYS_MAX=250;

// Rates of mutation in 1/X odds of happening per generation
unsigned POLYS_ADD_RATE=75;
unsigned POLYS_REMOVE_RATE=150;
unsigned POLYS_REORDER_RATE=50;
unsigned POINT_ADD_RATE=1500;
unsigned POINT_DELETE_RATE=1500;
unsigned POINT_MOVE_MAX_RATE=200;
unsigned POINT_MOVE_MED_RATE=50;
unsigned POINT_MOVE_MIN_RATE=150;
unsigned RED_CHANGE_RATE=100;
unsigned GREEN_CHANGE_RATE=100;
unsigned BLUE_CHANGE_RATE=100;
unsigned ALPHA_CHANGE_RATE=200;

// Ranges
unsigned POINT_MOVE_MED_RANGE=40;
unsigned POINT_MOVE_MIN_RANGE=6;
unsigned ALPHA_MIN=30;
unsigned ALPHA_MAX=100;

