#include "vertex.h"
#include<vector>

Vertex::Vertex(float xc, float yc, float zc, float r, float g, float b)
{
    this->coordinates[0] = xc;
    this->coordinates[1] = yc;
    this->coordinates[2] = zc;
    this->colour[0] = r;
    this->colour[1] = g;
    this->colour[2] = b;
}
