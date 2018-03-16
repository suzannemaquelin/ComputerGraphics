#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <QMatrix4x4>
#include <memory>


class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram shaderProgram_normal;
    QOpenGLShaderProgram shaderProgram_gouraud;
    QOpenGLShaderProgram shaderProgram_phong;

    GLint uniformModelViewTransform_normal, uniformProjectionTransform_normal, uniformNormal_transformation_normal;
    GLint uniformModelViewTransform_gouraud, uniformProjectionTransform_gouraud, uniformNormal_transformation_gouraud;
    GLint uniformModelViewTransform_phong, uniformProjectionTransform_phong, uniformNormal_transformation_phong;
    GLint uniformLightPosition_phong, uniformLightIntensity_phong;
    GLint uniformLightPosition_gouraud, uniformLightIntensity_gouraud;
    GLint uniformMaterialIa_phong, uniformMaterial_kd_phong, uniformMaterial_ka_phong, uniformMaterial_ks_phong, uniformPhongExp_phong;
    GLint uniformMaterialIa_gouraud, uniformMaterial_kd_gouraud, uniformMaterial_ka_gouraud, uniformMaterial_ks_gouraud, uniformPhongExp_gouraud;
    GLint uniformTexSampler_phong, uniformTexSampler_gouraud;

    // Mesh values
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;
    QMatrix4x4 meshTransform;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;

    //Textures
    GLuint texture;

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    ShadingMode shadingmode;

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);
    QVector<quint8> imageToBytes (QImage image);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();
    void prepareData();
    void loadMesh(QString model_file, GLuint* VAO, GLuint* VBO);
    void loadTexture(QString file, GLuint texturepointer);

    void paintPhong();
    void paintGouraud();
    void paintNormal();

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();
};

#endif // MAINVIEW_H
