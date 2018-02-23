#include "quad.h"

#include <cmath>

using namespace std;

Hit Quad::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    Hit h1 = t1.intersect();
    Hit h2 = t2.intersect();
    if( h1.NO_HIT() && h2.NO_HIT()) return h1;
    if (h1.NO_HIT()) return h2;
    return h1;
}

Quad::Quad(Triangle const &t1, Triangle const &t2)
:
    t1(t1),
    t2(t2)
{}
