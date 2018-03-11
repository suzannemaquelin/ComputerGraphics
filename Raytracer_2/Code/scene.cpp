#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::trace(Ray const &ray, int const reflectionDepth)
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t && hit.t > 0)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = &(obj->material);              //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    // Find color depending on the material having texture or not
    Color color;
    if (material->hasTexture == true) {
        float u, v;
        //pointmaping needs unit vector from hitpoint pointing to sphere's origin
        //this is exactly minus one times the normal vector
        std::tie(u,v) = obj->pointMapping((-1*N).normalized());
        Image im = material->texture;
        color = im.colorAt(u, v);
    } else {
        color = material->color;
    }
    //printf("color: %lf, %lf, %lf\n", color.x, color.y, color.z);

    /* Calculation of the color (Phong model) */

    Triple I_a = color * material->ka;
    Triple I_d, I_s;

    for (int i = 0; i < lights.size(); i++) {
        Triple L = (lights[i]->position) - (hit);
        L.normalize();
        Ray rayFromLight = Ray(lights[i]->position, (hit - lights[i]->position).normalized());
        if (shadowOn == false || getClosest(rayFromLight) == obj) {
            I_d += (lights[i]->color) * max(0.0, N.dot(L));

            double s = N.dot(L)*2;
            Triple R = (N*s) - L;
            R.normalize();

            double m = max(0.0, R.dot(V));
            I_s += lights[i]->color * pow(m, material->n);
        }
    }
    I_d = I_d * (material->kd) * (color);
    I_s = I_s * (material->ks);

    Color reflectionColor;
    if (reflectionDepth > 0 && material->ks > 0.0) {
        double s = N.dot(ray.D)*2;
        Triple R = ray.D - (N*s);
        R.normalize();
        reflectionColor = trace(Ray(hit + 0.1 * R, R), reflectionDepth-1);
        reflectionColor = reflectionColor * material->ks;
    }

    return I_a + I_d + I_s + reflectionColor;
}

ObjectPtr Scene::getClosest(Ray const &ray) {
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t && hit.t > 0)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }
    return obj;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();
    float step  = 1.0/(superSampling + 1);

    #pragma omp parallel num_threads(2)
    #pragma omp parallel for
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            Color col;
            for(float a = step; a < 1; a+=step) {
                for(float b = step; b < 1; b+=step) {
                    Point pixel(x + a, h - 1 - y + b, 0);
                    Ray ray(eye, (pixel - eye).normalized());
                    col += trace(ray, maxRecursionDepth);
                }
            }
            col /= (superSampling*superSampling);
            col.clamp();
            img(x, y) = col;
        }
    }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

void Scene::setShadow(bool const &shadow)
{
    shadowOn = shadow;
}

void Scene::setMaxRecursionDepth(int const &depth)
{
    maxRecursionDepth = depth;
}

void Scene::setSuperSampling(int const &sampling)
{
    superSampling = sampling;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}
