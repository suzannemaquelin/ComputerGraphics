#include "raytracer.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"
#include <tuple>

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/plane.h"
#include "shapes/quad.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node, string const &ifname)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];
        float angle = 0.0;

        auto angleStatus = node.find("angle");
        if (angleStatus != node.end()) {
            angle = node["angle"];
        }
        auto rotationStatus = node.find("rotation");
        if (rotationStatus != node.end()) {
            Point rotation(node["rotation"]);
            obj = ObjectPtr(new Sphere(pos, radius, rotation, angle));
        } else {
            obj = ObjectPtr(new Sphere(pos, radius, Point(0.0,0.0,0.0), angle));
        }

    } else if (node["type"] == "triangle") {
        Point vertex1(node["vertex1"]);
        Point vertex2(node["vertex2"]);
        Point vertex3(node["vertex3"]);
        obj = ObjectPtr(new Triangle(vertex1, vertex2, vertex3));
    } else if(node["type"] == "plane") {
        float a(node["a"]);
        float b(node["b"]);
        float c(node["c"]);
        float d(node["d"]);
        obj = ObjectPtr(new Plane(a, b, c, d));
    } else if(node["type"] == "quad") {
        Point vertex1(node["vertex1"]);
        Point vertex2(node["vertex2"]);
        Point vertex3(node["vertex3"]);
        Point vertex4(node["vertex4"]);
        obj = ObjectPtr(new Quad(vertex1, vertex2, vertex3, vertex4));
    }
    else
    {
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"], ifname);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node, string const &ifname) const
{
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];

    //find out to parse either a color or a texture
    auto colorStatus = node.find("color");
    if (colorStatus != node.end()) {
        Color color(node["color"]);
        return Material(color, ka, kd, ks, n, false);
    }

    auto textureStatus = node.find("texture");
    if (textureStatus != node.end()) {
        string s = node["texture"];
        //find right directory for texture file
        string ofname = ifname;
        // replace image file with texture file
        ofname.erase(ofname.begin() + ofname.find_last_of('/'), ofname.end());
        ofname += "/" + s;
        Image im(ofname);
        return Material(im, ka, kd, ks, n, true);
    }

    return Material(Color(), ka, kd, ks, n, false);
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    //try to find "Shadows", else set it to false
    auto shadowStatus = jsonscene.find("Shadows");
    if (shadowStatus != jsonscene.end()) {
        bool shadow(jsonscene["Shadows"]);
        scene.setShadow(shadow);
    } else {
        scene.setShadow(false);
    }

    //try to find "MaxRecursionDepth", else set it to zero
    auto maxrecdepthStatus = jsonscene.find("MaxRecursionDepth");
    if (maxrecdepthStatus != jsonscene.end()) {
        int maxRecursionDepth(jsonscene["MaxRecursionDepth"]);
        scene.setMaxRecursionDepth(maxRecursionDepth);
    } else {
        scene.setMaxRecursionDepth(0);
    }

    //try to find "SuperSamplingFactor", else set it to 1
    auto superSamplingStatus = jsonscene.find("SuperSamplingFactor");
    if (superSamplingStatus != jsonscene.end()) {
        int superSampling(jsonscene["SuperSamplingFactor"]);
        scene.setSuperSampling(superSampling);
    } else {
        scene.setSuperSampling(1);
    }

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);


    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode, ifname))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    // TODO: the size may be a settings in your file
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
