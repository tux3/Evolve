#include "poly.h"

Poly::Poly()
{

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
