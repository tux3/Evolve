#include "poly.h"

Poly::Poly()
{

}

void Poly::drawPoly(QImage& target, Poly& poly)
{
    static QBrush brush(Qt::SolidPattern);
    QPainter painter(&target);
    painter.setPen(QPen(Qt::NoPen));
    brush.setColor(poly.color);
    painter.setBrush(brush);
    painter.drawPolygon(poly.points.data(), poly.points.size());
}


bool Poly::operator==(const Poly& other)
{
    return color == other.color && points == other.points;
}

QDataStream& operator<< (QDataStream& stream, const Poly& poly)
{
    stream << poly.color;
    stream << poly.points;
    return stream;
}

QDataStream& operator>> (QDataStream& stream, Poly& poly)
{
    stream >> poly.color;
    stream >> poly.points;
    return stream;
}

bool Poly::hasPointIn(QRect rect)
{
    for (QPoint& point:points)
        if (rect.contains(point))
            return true;
    return false;
}
