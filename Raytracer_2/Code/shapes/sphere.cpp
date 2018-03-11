#include "sphere.h"
#include <tuple>

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

    double t1 = -term1 - sqrt(discriminant);
    double t2 = -term1 + sqrt(discriminant);
    t = min(t1, t2);
    if (t < 0){
        t = max(t1, t2);
    }

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

std::tuple<float, float> Sphere::pointMapping(Triple p) {
//    This is done through the simplest possible rotation and then the whole sphere
//    is rotated by angle degrees around the z-axis (note that this corresponds to
//    subtracting the angle in radians from the angle gotten by the arctan).
    float u = 0.5 + atan2(p.z,p.x) / (2*M_PI);
//    u = u - (this->a * (M_PI/180));
    float v = 0.5 + asin(p.y) / M_PI;
    return std::make_tuple(u,v);
}

Sphere::Sphere(Point const &pos, double radius, Point rotation, float angle)
:
    position(pos),
    r(radius),
    rot(rotation),
    a(angle)
{}
