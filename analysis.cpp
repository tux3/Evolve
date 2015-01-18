#include "analysis.h"
#include <QDebug>

QColor findDominantColor(QImage image)
{
    uint8_t* data = image.bits();
    quint64 size=image.byteCount();

    // First only keep the 3 highest bits of each channel (R, G, B)
    // Find the most common colors that all have the same 3 bits for each channel
    quint64 map1[8][8][8]={0};
    for (quint64 i=0; i<size; i+=4)
    {
        unsigned pixel = *(unsigned*)(data+i);
        map1[(pixel>>(16+5))&7][(pixel>>(8+5))&7][(pixel>>5)&7]++;
    }
    quint64 bestr=0,bestg=0,bestb=0,max=0;
    for (int i=0;i<8;i++)
    for (int j=0;j<8;j++)
    for (int k=0;k<8;k++)
    {
        if (map1[i][j][k] > max)
        {
            max=map1[i][j][k];
            bestr=i;
            bestg=j;
            bestb=k;
        }
    }

    // Now out of all the colors with those 3 high bits, take the average
    quint64 avgr=0, avgg=0, avgb=0, count=0;
    for (quint64 i=0; i<size; i+=4)
    {
        unsigned pixel = *(unsigned*)(data+i);
        int r = pixel>>16&0xFF, g=pixel>>8&0xFF, b=pixel&0xFF;
        if ((r>>5&7)==bestr && (g>>5&7)==bestg && (b>>5&7)==bestb)
        {
            avgr+=r;
            avgg+=g;
            avgb+=b;
            count++;
        }
    }
    return QColor(avgr/count, avgg/count, avgb/count);
}
