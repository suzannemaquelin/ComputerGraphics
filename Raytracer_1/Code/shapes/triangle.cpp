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
//    double t;
    Triple ray_origin = ray.O;
    Triple ray_direction = ray.D;

    const float EPSILON = 0.0000001;
    Triple edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex2 - vertex1;
    edge2 = vertex3 - vertex1;

    Triple N = edge1.cross(edge2);
    N.normalize();


    h = ray_direction.cross(edge2);
    a = edge1.dot(h);

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
    float t = f * edge2.dot(q);

    if (t > EPSILON) // ray intersection
    {
        //make sure that the angle between the normal and the incoming ray is less than 90 degrees
        if (N.dot(ray_direction)>0) {
            N = Triple(-1,-1,-1) * N;
        }
        fprintf(stderr, "printing the normal vector: %lf, %lf, %lf\n", N.x, N.y, N.z);

        return Hit(t,N);
    }
    else // This means that there is a line intersection but not a ray intersection.
        return Hit::NO_HIT();
}

Triangle::Triangle(Point const &v1, Point const &v2, Point const &v3)
:
    vertex1(v1),
    vertex2(v2),
    vertex3(v3)
{}
