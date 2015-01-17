#ifndef POLY_H
#define POLY_H

#include <QVector>
#include <QPair>
#include <QColor>
#include <QPoint>
#include <QDataStream>
#include <QPainter>
#include "stdint.h"


struct Poly
{
public:
    Poly();
    bool operator==(const Poly& other);
    static void drawPoly(QImage& target, Poly& poly);
    static void drawPoly(QImage& target, Poly& poly, QPainter& painter);
    bool hasPointIn(QRect rect);

public:
    QVector<QPoint> points;
    QColor color;
};

QDataStream& operator<< (QDataStream& stream, const Poly& poly);
QDataStream& operator>> (QDataStream& stream, Poly& poly);

#endif // POLY_H
