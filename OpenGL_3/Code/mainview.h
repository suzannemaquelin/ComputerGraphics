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

    GLint uniformModelTransform_normal, uniformViewTransform_normal, uniformProjectionTransform_normal, uniformNormal_transformation_normal;
    GLint uniformModelTransform_gouraud, uniformViewTransform_gouraud, uniformProjectionTransform_gouraud, uniformNormal_transformation_gouraud;
    GLint uniformModelTransform_phong, uniformViewTransform_phong, uniformProjectionTransform_phong, uniformNormal_transformation_phong;
    GLint uniformLightPosition_phong, uniformLightIntensity_phong;
    GLint uniformLightPosition_gouraud, uniformLightIntensity_gouraud;
    GLint uniformMaterialIa_phong, uniformMaterial_kd_phong, uniformMaterial_ka_phong, uniformMaterial_ks_phong, uniformPhongExp_phong;
    GLint uniformMaterialIa_gouraud, uniformMaterial_kd_gouraud, uniformMaterial_ka_gouraud, uniformMaterial_ks_gouraud, uniformPhongExp_gouraud;
    GLint uniformTexSampler_phong, uniformTexSampler_gouraud;

    // Mesh values
//    GLuint meshVAO;
//    GLuint meshVBO;
//    GLuint meshSize;
//    QMatrix4x4 meshTransform;

//    GLuint sphereVAO;
//    GLuint  sphereVBO;
//    GLuint  sphereSize;
//    QMatrix4x4  sphereTransform;

    // Transforms
    float scale = 1.f;
    float rotation_addition = 0.0;
    QVector3D translation;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;
    QMatrix4x4 viewTransform;

    //Textures
    GLuint texture;
    GLuint texture2;

    struct mesh {
        GLuint VAO;
        GLuint VBO;
        GLuint size;
        QMatrix4x4 transform;

        // Transforms
        float scale = 1.f;
        float rotation_addition = 0.0;
        QVector3D translation_addition = QVector3D(0.0,0.0,0.0);
        QVector3D translation;
        QVector3D rotation;

        //Textures
        GLuint texture;
    };
    mesh* meshes;
    int noMeshes;

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
    void loadMesh(QString model, GLuint VAO, GLuint VBO, GLuint* size);
    void loadTexture(QString file, GLuint texturepointer);
    void setTransform(QVector3D trans, float addition, mesh* m);

    void paintPhong();
    void paintGouraud();
    void paintNormal();

    void drawMesh(mesh m, GLuint uniformModel_Transform, GLuint uniformNormal_Transorm);
    void updatePositions();

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateViewTransform();
    void updateModelTransforms(QMatrix4x4* transform, QVector3D translation, float scale, QVector3D rotation);

};

#endif // MAINVIEW_H
