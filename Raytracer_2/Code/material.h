#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "triple.h"
#include "image.h"

class Material
{
    public:
        Image texture;      // texture
        Color color;        // base color
        double ka;          // ambient intensity
        double kd;          // diffuse intensity
        double ks;          // specular intensity
        double n;           // exponent for specular highlight size
        bool hasTexture;    // boolean specifying to use color or texture

        Material() = default;

        Material(Color const &color, double ka, double kd, double ks, double n, bool hasTexture)
        :
            color(color),
            ka(ka),
            kd(kd),
            ks(ks),
            n(n),
            hasTexture(hasTexture)
        {}

        Material(Image const &texture, double ka, double kd, double ks, double n, bool hasTexture)
        :
            texture(texture),
            ka(ka),
            kd(kd),
            ks(ks),
            n(n),
            hasTexture(hasTexture)
        {}
};

#endif
