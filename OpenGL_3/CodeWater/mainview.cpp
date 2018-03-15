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

    prepareData();

    // Initialize transformations
    viewTransform.setToIdentity();
    updateProjectionTransform();
    for (int m = 0; m < noMeshes; m++) {
        printf("translation mesh: %lf %lf %lf\n", (meshes[m].translation).x(),(meshes[m].translation).y(), (meshes[m].translation).z());
        updateModelTransforms(&(meshes[m].transform), meshes[m].translation, meshes[m].scale, meshes[m].rotation);
    }
    shadingmode = NORMAL;

    timer.start(1000.0 / 60.0);
}

void MainView::createShaderProgram()
{
    // Create normal shader program
    shaderProgram_normal.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    shaderProgram_normal.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    shaderProgram_normal.link();

    // Get the uniforms of normal
    uniformModelTransform_normal = shaderProgram_normal.uniformLocation("modelTransform");
    uniformViewTransform_normal = shaderProgram_normal.uniformLocation("viewTransform");
    uniformProjectionTransform_normal = shaderProgram_normal.uniformLocation("projectionTransform");
    uniformNormal_transformation_normal = shaderProgram_normal.uniformLocation("normal_transformation");
}

void MainView::prepareData()
{
    noMeshes = 1;
    meshes = (mesh*) malloc(noMeshes*sizeof(struct mesh));

    mesh grid;
    glGenVertexArrays(1, &(grid.VAO));
    glGenBuffers(1, &(grid.VBO));
    loadMesh(":/models/grid.obj", grid.VAO, grid.VBO, &(grid.size));
//    glGenTextures(1, &(grid.texture));
//    loadTexture(":/textures/cat_diff.png", grid.texture);
    setTransform(QVector3D(0.0,0.0,-5.0), 1.0, &grid);
    meshes[0] = grid;
}

void MainView::setTransform(QVector3D trans, float addition, mesh* m){
    (*m).translation = trans;
    //translation = QVector3D(0, 0.0, 0.0);
    (*m).rotation = QVector3D(0.0, 0.0, 0.0);
    (*m).scale = 1.0;
    (*m).rotation_addition = addition;
}

void MainView::loadMesh(QString model_file, GLuint VAO, GLuint VBO, GLuint* size)
{
    Model model(model_file);
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

    (*size) = vertexCoords.size();

    // bind VAO
    glBindVertexArray(VAO);

    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

    //update positions/orientation of objects
//    updatePositions();

//    switch(shadingmode) {
//        case PHONG:
//            paintPhong();
//            break;
//        case NORMAL:
            paintNormal();
//            break;
//        case GOURAUD:
//            paintGouraud();
//            break;
//    }
}

void MainView::updatePositions()
{
    for (int m = 0; m < noMeshes; m++) {
        if (m == 0) {
            meshes[m].transform.rotate(meshes[m].rotation_addition, QVector3D(1.0, 1.0, 0.0));
            meshes[m].transform.translate(QVector3D(0.01, 0.0, 0.0));
        } else {
            meshes[m].transform.rotate(meshes[m].rotation_addition, QVector3D(0.0, 1.0, 0.0));
        }
    }
}

void MainView::paintNormal()
{
    shaderProgram_normal.bind();

    // Set the projection matrix
    glUniformMatrix4fv(uniformProjectionTransform_normal, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformViewTransform_normal, 1, GL_FALSE, viewTransform.data());

    // Set the meshdata
    for (int m = 0; m < noMeshes; m++) {
        drawMesh(meshes[m], uniformModelTransform_normal, uniformNormal_transformation_normal);
    }

    shaderProgram_normal.release();
}

void MainView::drawMesh(mesh m, GLuint uniformModelTransform, GLuint uniformNormal_Transform)
{
    //bind vao of object
    glBindVertexArray(m.VAO);

    //bind texture of object
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.texture);

    //set uniforms for object
    QMatrix3x3 normal_transforming = m.transform.normalMatrix();
    glUniformMatrix4fv(uniformModelTransform, 1, GL_FALSE, m.transform.data());
    glUniformMatrix3fv(uniformNormal_Transform, 1, GL_FALSE, normal_transforming.data());

    //draw object
    glDrawArrays(GL_TRIANGLES, 0, m.size);
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

void MainView::updateViewTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    viewTransform.setToIdentity();
    viewTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms(QMatrix4x4* transform, QVector3D ob_translation, float ob_scale, QVector3D ob_rotation)
{
    (*transform).setToIdentity();
    (*transform).translate(ob_translation);
    (*transform).scale(ob_scale);
    (*transform).rotate(QQuaternion::fromEulerAngles(ob_rotation));

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    for(int m = 0; m < noMeshes; m++) {
        glDeleteBuffers(1, &(meshes[m].VBO));
        glDeleteVertexArrays(1, &(meshes[m].VAO));
        glDeleteTextures(1, &(meshes[m].texture));
    }
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    rotation_view = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    viewTransform.setToIdentity();
    viewTransform.rotate(QQuaternion::fromEulerAngles(rotation_view));
    viewTransform.translate(translation_view);

    update();
//    updateModelTransforms(&meshTransform, translation, scale, rotation);
}

void MainView::setTranslationView(int translateX, int translateY, int translateZ)
{
    viewTransform.setToIdentity();
    viewTransform.rotate(QQuaternion::fromEulerAngles(rotation_view));
    translation_view += { static_cast<float>(translateX), static_cast<float>(translateY), static_cast<float>(translateZ)};
    viewTransform.translate(translation_view);
    update();
}

void MainView::setScale(int newScale)
{
    scale = static_cast<float>(newScale) / 100.f;
//    updateModelTransforms(&meshTransform, translation, scale, rotation);
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

