#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray)
{
    Triple ray_origin = ray.O;
    Triple ray_direction = ray.D;

    const float EPSILON = 1e-6;
    Triple edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex2 - vertex1;
    edge2 = vertex3 - vertex1;
    h = ray_direction.cross(edge2);
    a = edge1.dot(h);

    Triple N = edge1.cross(edge2);
    N.normalize();

    //ray and triangle are parallel
    if (a > -EPSILON && a < EPSILON)
        return Hit::NO_HIT();

    f = 1/a;
    s = ray_origin - vertex1;
    u = f * (s.dot(h));
    if (u < 0.0 || u > 1.0)
        return Hit::NO_HIT();

    q = s.cross(edge1);
    v = f * ray_direction.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return Hit::NO_HIT();

    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = f * edge2.dot(q);

    if (t > EPSILON) // ray intersection
    {
        //The degree between the normal and the vector ray direction should be larger than 90 degrees
        if (N.dot(ray_direction) > 0) {
            N = Triple(-1,-1,-1) * N;
        }

        return Hit(t,N);
    }
    else // This means that there is a line intersection but not a ray intersection.
        return Hit::NO_HIT();
}

std::tuple<float, float> Triangle::pointMapping(Triple p) {
    //trivial implementation
    if (p.x > 1.0 || p.y > 1.0) {
        return std::make_tuple(0.0, 0.0);
    } else {
        return std::make_tuple(p.x, p.y);
    }
}

Triangle::Triangle(Point const &v1, Point const &v2, Point const &v3)
:
    vertex1(v1),
    vertex2(v2),
    vertex3(v3)
{}
