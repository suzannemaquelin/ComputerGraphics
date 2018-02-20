#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray)
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
    double t;
    Triple o = ray.O;
    Triple l = ray.D;

    Triple v0v1 = v1 - v0;
    Triple v0v2 = v2 - v0;
    Triple N = v0v1.cross(v0v2);


    // check if ray and plane are parallel ?
    float dot_N_l = N.dot(l);
    if (dot_N_l == 0) {
        return Hit::NO_HIT();
    }

    float d = N.dot(v0);

    // compute t (equation 3)
    t = (N.dot(o) + d) / dot_N_l;

    return Hit(t,N);
}

Triangle::Triangle(Point v1, Point v2, Point v3)
:
    vertex1(v1),
    vertex2(v2),
    vertex3(v3)
{}
