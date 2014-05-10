#include "poly.h"

Poly::Poly()
{

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
