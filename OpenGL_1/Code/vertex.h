#ifndef VERTEX_H
#define VERTEX_H
#include<vector>


class Vertex
{
public:
    Vertex(float x, float y, float z, float red, float green, float blue);
    float coordinates[3];
    float colour[3];
};


#endif // VERTEX_H
