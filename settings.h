#ifndef SETTINGS_H
#define SETTINGS_H

extern unsigned N_POLY_POINTS;
extern unsigned N_COLOR_VAR;
extern unsigned N_POS_VAR;

extern unsigned GUI_REFRESH_RATE;

extern unsigned AUTOFOCUS_RES;
extern unsigned FOCUS_LEFT, FOCUS_RIGHT, FOCUS_TOP, FOCUS_BOTTOM;

extern unsigned N_CORES;

extern unsigned POLYS_MIN, POLYS_MAX;

extern unsigned POLYS_ADD_RATE, POLYS_REMOVE_RATE, POLYS_REORDER_RATE;
extern unsigned POINT_MOVE_MAX_RATE, POINT_MOVE_MED_RATE, POINT_MOVE_MIN_RATE;
extern unsigned POINT_MOVE_MED_RANGE, POINT_MOVE_MIN_RANGE;
extern unsigned POINT_ADD_RATE, POINT_DELETE_RATE;
extern unsigned RED_CHANGE_RATE, GREEN_CHANGE_RATE, BLUE_CHANGE_RATE, ALPHA_CHANGE_RATE;
extern unsigned ALPHA_MIN, ALPHA_MAX;

#define GEN_WITH_RANDOM_COLOR false
#define OPT_INCREASE_ALPHA true

#endif // SETTINGS_H
