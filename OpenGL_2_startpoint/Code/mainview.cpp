#include "mainview.h"
#include "model.h"
#include "vertex.h"

#include <math.h>
#include <QDateTime>


/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    makeCurrent();
    debugLogger->stopLogging();
    delete debugLogger;

    qDebug() << "MainView destructor";
    destroyModelBuffers();
    doneCurrent();
}

QVector<quint8> MainView::imageToBytes(QImage image) {
    // needed since (0,0) is bottom left in OpenGL
    QImage im = image.mirrored();
    QVector<quint8> pixelData;
    pixelData.reserve(im.width()*im.height()*4);

    for (int i = 0; i != im.height(); ++i) {
        for (int j = 0; j != im.width(); ++j) {
            QRgb pixel = im.pixel(j,i);

            // pixel is of format #AARRGGBB (in hexadecimal notation)
            // so with bitshifting and binary AND you can get
            // the values of the different components
            quint8 r = (quint8)((pixel >> 16) & 0xFF); // Red component
            quint8 g = (quint8)((pixel >> 8) & 0xFF); // Green component
            quint8 b = (quint8)(pixel & 0xFF); // Blue component
            quint8 a = (quint8)((pixel >> 24) & 0xFF); // Alpha component

            // Add them to the Vector
            pixelData.append(r);
            pixelData.append(g);
            pixelData.append(b);
            pixelData.append(a);
        }
    }
    return pixelData;
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    createShaderProgram();
    loadMesh();

    glGenTextures(1, &texture);
    loadTexture(":/textures/cat_diff.png", texture);

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
    shadingmode = PHONG;
}

void MainView::createShaderProgram()
{
    // Create normal shader program
    shaderProgram_normal.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    shaderProgram_normal.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    shaderProgram_normal.link();

    // Create gouraud shader program
    shaderProgram_gouraud.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    shaderProgram_gouraud.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    shaderProgram_gouraud.link();

    // Create phong shader program
    shaderProgram_phong.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    shaderProgram_phong.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    shaderProgram_phong.link();

    // Get the uniforms of normal
    uniformModelViewTransform_normal = shaderProgram_normal.uniformLocation("modelViewTransform");
    uniformProjectionTransform_normal = shaderProgram_normal.uniformLocation("projectionTransform");
    uniformNormal_transformation_normal = shaderProgram_normal.uniformLocation("normal_transformation");

    // Get the uniforms of gouraud
    uniformModelViewTransform_gouraud = shaderProgram_gouraud.uniformLocation("modelViewTransform");
    uniformProjectionTransform_gouraud = shaderProgram_gouraud.uniformLocation("projectionTransform");
    uniformNormal_transformation_gouraud = shaderProgram_gouraud.uniformLocation("normal_transformation");
    uniformLightPosition_gouraud = shaderProgram_gouraud.uniformLocation("light_position");
    uniformLightIntensity_gouraud = shaderProgram_gouraud.uniformLocation("light_intensity");
    uniformMaterialIa_gouraud = shaderProgram_gouraud.uniformLocation("material_Ia");
    uniformMaterial_ka_gouraud = shaderProgram_gouraud.uniformLocation("material_ka");
    uniformMaterial_kd_gouraud = shaderProgram_gouraud.uniformLocation("material_kd");
    uniformMaterial_ks_gouraud = shaderProgram_gouraud.uniformLocation("material_ks");
    uniformPhongExp_gouraud = shaderProgram_gouraud.uniformLocation("phongExp");
    uniformTexSampler_gouraud = shaderProgram_gouraud.uniformLocation("textureSampler");

    // Get the uniforms of phong
    uniformModelViewTransform_phong = shaderProgram_phong.uniformLocation("modelViewTransform");
    uniformProjectionTransform_phong = shaderProgram_phong.uniformLocation("projectionTransform");
    uniformNormal_transformation_phong = shaderProgram_phong.uniformLocation("normal_transformation");
    uniformLightPosition_phong = shaderProgram_phong.uniformLocation("light_position");
    uniformLightIntensity_phong = shaderProgram_phong.uniformLocation("light_intensity");
    uniformMaterialIa_phong = shaderProgram_phong.uniformLocation("material_Ia");
    uniformMaterial_ka_phong = shaderProgram_phong.uniformLocation("material_ka");
    uniformMaterial_kd_phong = shaderProgram_phong.uniformLocation("material_kd");
    uniformMaterial_ks_phong = shaderProgram_phong.uniformLocation("material_ks");
    uniformPhongExp_phong = shaderProgram_phong.uniformLocation("phongExp");
    uniformTexSampler_phong = shaderProgram_phong.uniformLocation("textureSampler");
}

void MainView::loadMesh()
{
    Model model(":/models/cat.obj");
    QVector<QVector3D> vertexCoords = model.getVertices();
    QVector<QVector3D> vertexNormals = model.getNormals();
    QVector<QVector2D> textureCoords = model.getTextureCoords();

    QVector<float> meshData;
    meshData.reserve(2 * 3 * vertexCoords.size() + 2 * textureCoords.size());

    for (int i = 0; i < vertexCoords.length(); i++) {
        QVector3D coord = vertexCoords[i];
        QVector3D normals = vertexNormals[i];
        QVector2D tex = textureCoords[i];
        meshData.append(coord.x());
        meshData.append(coord.y());
        meshData.append(coord.z());
        meshData.append(normals.x());
        meshData.append(normals.y());
        meshData.append(normals.z());
        meshData.append(tex.x());
        meshData.append(tex.y());
    }

    meshSize = vertexCoords.size();

    // Generate VAO
    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    // Generate VBO
    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set colour coordinates to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MainView::loadTexture(QString file, GLuint texturepointer){
    QImage image = QImage(file);
    QVector<quint8> texture_image = imageToBytes(image);

    glBindTexture(GL_TEXTURE_2D, texturepointer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //are width and height correct???
    GLsizei width = image.width();
    GLsizei height = image.height();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_image.data());
    glGenerateMipmap(GL_TEXTURE_2D);
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    switch(shadingmode) {
        case PHONG:
            paintPhong();
            break;
        case NORMAL:
            paintNormal();
            break;
        case GOURAUD:
            paintGouraud();
            break;
    }
}

void MainView::paintPhong()
{
    shaderProgram_phong.bind();

    QMatrix3x3 normal_transforming = meshTransform.normalMatrix();

    // Set the projection matrix
    glUniformMatrix4fv(uniformProjectionTransform_phong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransform_phong, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormal_transformation_phong, 1, GL_FALSE, normal_transforming.data());

    // Set the lightdata
    QVector3D light_position = QVector3D(1.0, 0.0, 0.0);
    QVector3D light_intensity = QVector3D(0.0, 0.0, 1.0);
    glUniform3f(uniformLightPosition_phong, light_position[0], light_position[1], light_position[2]);
    glUniform3f(uniformLightIntensity_phong, light_intensity[0], light_intensity[1], light_intensity[2]);

    // Set the materialdata
    QVector3D material_Ia = QVector3D(1.0, 1.0, 1.0);
    QVector3D material_kd = QVector3D(0.1, 0.5, 0.8);
    QVector3D material_ka = QVector3D(0.7, 0.7, 0.7);
    QVector3D material_ks = QVector3D(1.0, 1.0, 1.0);
    float phongExp = 32.0;

    glUniform3f(uniformMaterialIa_phong, material_Ia[0], material_Ia[1], material_Ia[2]);
    glUniform3f(uniformMaterial_kd_phong, material_kd[0], material_kd[1], material_kd[2]);
    glUniform3f(uniformMaterial_ka_phong, material_ka[0], material_ka[1], material_ka[2]);
    glUniform3f(uniformMaterial_ks_phong, material_ks[0], material_ks[1], material_ks[2]);
    glUniform1f(uniformPhongExp_phong, phongExp);
//    uniformTexSampler_phong

    // Set the meshdata
    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    shaderProgram_phong.release();
}

void MainView::paintGouraud()
{
    shaderProgram_gouraud.bind();

    QMatrix3x3 normal_transforming = meshTransform.normalMatrix();

    // Set the projection matrix
    glUniformMatrix4fv(uniformProjectionTransform_gouraud, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransform_gouraud, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormal_transformation_gouraud, 1, GL_FALSE, normal_transforming.data());

    // Set the lightdata
    QVector3D light_position = QVector3D(1.0, 0.0, 0.0);
    QVector3D light_intensity = QVector3D(0.0, 0.0, 1.0);
    glUniform3f(uniformLightPosition_gouraud, light_position[0], light_position[1], light_position[2]);
    glUniform3f(uniformLightIntensity_gouraud, light_intensity[0], light_intensity[1], light_intensity[2]);

    // Set the materialdata
    QVector3D material_Ia = QVector3D(1.0, 1.0, 1.0);
    QVector3D material_kd = QVector3D(0.1, 0.5, 0.8);
    QVector3D material_ka = QVector3D(0.7, 0.7, 0.7);
    QVector3D material_ks = QVector3D(1.0, 1.0, 1.0);
    float phongExp = 32.0;

    glUniform3f(uniformMaterialIa_gouraud, material_Ia[0], material_Ia[1], material_Ia[2]);
    glUniform3f(uniformMaterial_kd_gouraud, material_kd[0], material_kd[1], material_kd[2]);
    glUniform3f(uniformMaterial_ka_gouraud, material_ka[0], material_ka[1], material_ka[2]);
    glUniform3f(uniformMaterial_ks_gouraud, material_ks[0], material_ks[1], material_ks[2]);
    glUniform1f(uniformPhongExp_gouraud, phongExp);
//    uniformTexSampler_gouraud

    // Set the meshdata
    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    shaderProgram_gouraud.release();
}

void MainView::paintNormal()
{
    shaderProgram_normal.bind();

    QMatrix3x3 normal_transforming = meshTransform.normalMatrix();

    // Set the projection matrix
    glUniformMatrix4fv(uniformProjectionTransform_normal, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransform_normal, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormal_transformation_normal, 1, GL_FALSE, normal_transforming.data());

    // Set the meshdata
    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    shaderProgram_normal.release();
}


/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    updateProjectionTransform();
}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms()
{
    meshTransform.setToIdentity();
    meshTransform.translate(0, -0.2, -1);
    meshTransform.scale(scale);
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &meshVAO);
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    rotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    updateModelTransforms();
}

void MainView::setScale(int newScale)
{
    scale = static_cast<float>(newScale) / 100.f;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading)
{
    shadingmode = shading;
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}

