#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
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
    Triple center = position;
    Triple dir = ray.D;
    Triple oc = ray.O - center;

    double term1 = dir.dot(ray.O - center);
    double term2 = (ray.O - center).length_2();
    double discriminant = term1*term1 - term2 + r*r;

    if (discriminant < 0) {
        return Hit::NO_HIT();
    }

    t = -term1 - sqrt(discriminant);

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    Triple intersection = ray.O +(dir * t);
    Vector N = intersection - center;
    N.normalize();

    return Hit(t,N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
