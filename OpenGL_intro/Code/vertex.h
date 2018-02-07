#ifndef VERTEX_H
#define VERTEX_H
#include<vector>


class Vertex
{
public:
    Vertex(float x, float y, float red, float green, float blue);
    float coordinates[2];
    float colour[3];
};


#endif // VERTEX_H
