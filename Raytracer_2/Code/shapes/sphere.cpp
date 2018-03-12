#include "sphere.h"
#include <tuple>

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    //INTERSECTION CALCULATION
    double t;
    Triple center = position;

    double term1 = (ray.D).dot(ray.O - center);
    double term2 = (ray.O - center).length_2();
    double discriminant = term1*term1 - term2 + r*r;

    if (discriminant < 0) {
        return Hit::NO_HIT();
    }

    //find out which hit point is closest and in front of eye
    double t1 = -term1 - sqrt(discriminant);
    double t2 = -term1 + sqrt(discriminant);
    t = min(t1, t2);
    if (t < 0){
        t = max(t1, t2);
    }

    //NORMAL CALCULATION

    Triple intersection = ray.O +(ray.D * t);
    Vector N = intersection - center;
    N.normalize();

    return Hit(t,N);
}

std::tuple<float, float> Sphere::pointMapping(Triple p) {
    Triple rotation = rot.normalized();
    float angle = a * M_PI / 180; //in radians
    //according to Rodrigues' rotation formula
    Triple rotated = p * cos(angle) + rotation.cross(p) * sin(angle) + rotation * (rotation.dot(p)) * (1 - cos(angle));

    p = rotated.normalized();
    float u = 0.5 + atan2(p.y,p.x) / (2*M_PI);
    float v = 0.5 + asin(p.z) / M_PI;
    return std::make_tuple(u,v);
}

Sphere::Sphere(Point const &pos, double radius, Point rotation, float angle)
:
    position(pos),
    r(radius),
    rot(rotation),
    a(angle)
{}
