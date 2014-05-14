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
#if (useConvexPolys)
    painter.drawConvexPolygon(poly.points.data(), poly.points.size());
#else
    painter.drawPolygon(poly.points.data(), poly.points.size());
#endif
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
