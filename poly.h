#ifndef POLY_H
#define POLY_H

#include <QVector>
#include <QPair>
#include <QColor>
#include <QPoint>
#include <QDataStream>
#include "stdint.h"


struct Poly
{
public:
    Poly();
    bool operator==(const Poly& other);

public:
    QVector<QPoint> points;
    QColor color;
    int resizeTimes=0;
};

QDataStream& operator<< (QDataStream& stream, const Poly& poly);
QDataStream& operator>> (QDataStream& stream, Poly& poly);

#endif // POLY_H
