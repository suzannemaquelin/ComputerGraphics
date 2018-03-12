#include "quad.h"
#include "triangle.h"

#include <cmath>

using namespace std;

Hit Quad::intersect(Ray const &ray)
{
    /**
     * In order to draw a quad, we divide the quad into 2 triangles and compute the intersection
     * To find out which vertices we need for the 2 triangles we take the vertex 1 and compute which
     * vertex lies farthest from vertex 1. We will make a triangle not including this point and we will
     * make a second triangle including this point and the two vertices which lie nearest to it.
     */
    Triangle t1 = Triangle(v1, v2, v3);
    Triangle t2 = Triangle(v1, v2, v3);

    Point furthest_v1 = furthest_point(v1, v2, v3, v4);

    t1 = take_nearest(furthest_v1, v1, v2, v3, v4);

    if(furthest_v1.equals(v2)){
        Point furthest_v2 = furthest_point(v2, v1, v3, v4);
        t2 = take_nearest(furthest_v2, v2, v1, v3, v4);
    } else if(furthest_v1.equals(v3)){
        Point furthest_v3 = furthest_point(v3, v1, v2, v4);
        t2 = take_nearest(furthest_v3, v3, v1, v2, v4);
    } else {
        Point furthest_v4 = furthest_point(v4, v1, v2, v3);
        t2 = take_nearest(furthest_v4, v4, v1, v2, v3);
    }

    Hit h1 = t1.intersect(ray);
    Hit h2 = t2.intersect(ray);

    //Find out if t1 or t2 is being hit by the light
    if ( !isnan(h1.t)) {
        return h1;
    }
    if ( !isnan(h2.t)) {
        return h2;
    }

    return Hit::NO_HIT();
}

//furthest_point takes 4 points as input and computes which of point lies furthest from the first given point
Point Quad::furthest_point(Point const &v1, Point const &v2, Point const &v3, Point const &v4)
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

//This function takes the nearest two points to the triangle point and returns this triangle
Triangle Quad::take_nearest(Point furthest, Point triangle_point, Point p1, Point p2, Point p3) {
    if(furthest.equals(p1)) {
        return Triangle(triangle_point, p2, p3);
    } else if(furthest.equals(p2)) {
        return Triangle(triangle_point, p1, p3);
    } else {
        return Triangle(triangle_point, p1, p2);
    }

}

std::tuple<float, float> Quad::pointMapping(Triple p) {
    //trivial implementation
    if (p.x > 1.0 || p.y > 1.0) {
        return std::make_tuple(0.0, 0.0);
    } else {
        return std::make_tuple(p.x, p.y);
    }
}

Quad::Quad(Point const &v1, Point const &v2, Point const &v3, Point const &v4)
: v1(v1),
  v2(v2),
  v3(v3),
  v4(v4)
{}
