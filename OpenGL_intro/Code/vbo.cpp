#include "vbo.h"
#include<QOpenGLShaderProgram>

VBO::VBO()
{
    QOpenGLShader::Vertex v = addShaderFromSourceFile(Vertex, :/shaders/vertshader.glsl);

}

