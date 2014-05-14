#ifndef STATS_H
#define STATS_H

// Total number of mutations tried
extern unsigned STAT_POLY_ADD, STAT_POLY_REMOVE, STAT_POLY_REORDER;
extern unsigned STAT_POINT_MOVE_MAX, STAT_POINT_MOVE_MED, STAT_POINT_MOVE_MIN;

// Number of mutations accepted
extern unsigned STAT_POLY_ADD_OK, STAT_POLY_REMOVE_OK, STAT_POLY_REORDER_OK;
extern unsigned STAT_POINT_MOVE_MAX_OK, STAT_POINT_MOVE_MED_OK, STAT_POINT_MOVE_MIN_OK;

#endif // STATS_H
