#include "quad.h"
#include "triangle.h"

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

    Triangle t1 = Triangle(v1, v2, v3);
    Triangle t2 = Triangle(v1, v2, v3);
    Point furthest_v1 = function(v1, v2, v3, v4);
//    fprintf(stderr, "furthest from v1: %lf, %lf, %lf\n", furthest_v1.x, furthest_v1.y, furthest_v1.z);
    if(furthest_v1.equals(v2)){
        t1 = Triangle(v1, v3, v4);
        Point furthest_v2 = function(v2, v1, v3, v4);
        if(furthest_v2.equals(v1)) t2 = Triangle(v2, v3, v4);
        else if(furthest_v2.equals(v3)) t2 = Triangle(v2, v1, v4);
        else t2 = Triangle(v2, v1, v3);
    } else if(furthest_v1.equals(v3)){
        t1 = Triangle(v1, v2, v4);
        Point furthest_v3 = function(v3, v1, v2, v4);
        if(furthest_v3.equals(v1)) t2 = Triangle(v3, v2, v4);
        else if(furthest_v3.equals(v2)) t2 = Triangle(v3, v1, v4);
        else t2 = Triangle(v3, v1, v2);
    } else {
        t1 = Triangle(v1, v2, v3);
        Point furthest_v4 = function(v4, v1, v2, v3);
        if(furthest_v4.equals(v1)) t2 = Triangle(v4, v2, v3);
        else if(furthest_v4.equals(v2)) t2 = Triangle(v4, v1, v3);
        else t2 = Triangle(v4, v1, v2);
    }

    Hit h1 = t1.intersect(ray);
    Hit h2 = t2.intersect(ray);
//    if( h1.NO_HIT() && h2.NO_HIT()) return h1;
//    if (h1.NO_HIT()) return h2;
//    return h1;

    if ( !isnan(h1.t)) {
        return h1;
    }
    if ( !isnan(h2.t)) {
        return h2;
    }

    return Hit::NO_HIT();
}

Point Quad::function(Point const &v1, Point const &v2, Point const &v3, Point const &v4)
{
    double distance12 = v1.distance(v2);
    double distance13 = v1.distance(v3);
    double distance14 = v1.distance(v4);

    //check which vertice is furthest away from vertex 1 and make first triangle accordingly
    if(distance12 > distance13 && distance12 > distance14) {
        return v2;
    }
    else if(distance13 > distance12 && distance13 > distance14) {
        return v3;
    }
    else {
        return v4;
    }
}

Quad::Quad(Point const &v1, Point const &v2, Point const &v3, Point const &v4)
: v1(v1),
  v2(v2),
  v3(v3),
  v4(v4)
{}
