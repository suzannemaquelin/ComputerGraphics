#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <vector>

// Forward declerations
class Ray;
class Image;

class Scene
{
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    Point eye;
    bool shadowOn;
    int maxRecursionDepth;
    int superSampling;

    public:

        // trace a ray into the scene and return the color
        Color trace(Ray const &ray, int const reflectionDepth);
        Color getReflection(Ray const &ray, int const reflectionDepth);

        // render the scene to the given image
        void render(Image &img);

        ObjectPtr getClosest(Ray const &ray);

        void addObject(ObjectPtr obj);
        void addLight(Light const &light);
        void setEye(Triple const &position);
        void setShadow(bool const &shadow);
        void setMaxRecursionDepth(int const &depth);
        void setSuperSampling(int const &sampling);

        unsigned getNumObject();
        unsigned getNumLights();
};

#endif
