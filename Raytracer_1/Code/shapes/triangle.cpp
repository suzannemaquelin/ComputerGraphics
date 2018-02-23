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

//    Triple v1v2 = vertex2 - vertex1;
//    Triple v1v3 = vertex3 - vertex1;
//    Triple N = v1v2.cross(v1v3);


//    // check if ray and plane are perpendicular ?
//    float dot_N_l = N.dot(ray_direction);
//    if (dot_N_l == 0) {
//        return Hit::NO_HIT();
//    }

//    //distance from the origin (0, 0, 0)
//    //to the plane (if we trace a line from the origin to the plane, parallel to the plane's normal)
//    float d = N.dot(vertex1);
//    t = (N.dot(ray_origin) + d) / dot_N_l;

//    return Hit(t,N);


    const float EPSILON = 0.0000001;
    Triple edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex2 - vertex1;
    edge2 = vertex3 - vertex1;

    Triple N = edge1.cross(edge2);
    //N.normalize();
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
        //outIntersectionPoint = ray_origin + ray_direction * t;
        printf("t: %lf\n", t);
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
