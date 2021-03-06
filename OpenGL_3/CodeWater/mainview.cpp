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
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    createShaderProgram();
    prepareData();

    time = 0.0;

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
    timer.start(1000.0/60.0);
}

void MainView::createShaderProgram()
{
    // Create normal shader program
    shaderProgram_water.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_water.glsl");
    shaderProgram_water.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_water.glsl");
    shaderProgram_water.link();

    // Get the uniforms of normal
    uniformModelViewTransform = shaderProgram_water.uniformLocation("modelViewTransform");
    uniformProjectionTransform = shaderProgram_water.uniformLocation("projectionTransform");
    uniformNormal_transformation = shaderProgram_water.uniformLocation("normal_transformation");
    uniformAmplitude = shaderProgram_water.uniformLocation("amplitude");
    uniformFrequency = shaderProgram_water.uniformLocation("frequency");
    uniformPhase = shaderProgram_water.uniformLocation("phase");
    uniformNumberWaves = shaderProgram_water.uniformLocation("numberOfWaves");
    uniformTime = shaderProgram_water.uniformLocation("time");

    uniformLightPosition = shaderProgram_water.uniformLocation("light_position");
    uniformLightIntensity = shaderProgram_water.uniformLocation("light_intensity");
    uniformMaterialIa = shaderProgram_water.uniformLocation("material_Ia");
    uniformMaterial_ka = shaderProgram_water.uniformLocation("material_ka");
    uniformMaterial_kd = shaderProgram_water.uniformLocation("material_kd");
    uniformMaterial_ks = shaderProgram_water.uniformLocation("material_ks");
    uniformPhongExp = shaderProgram_water.uniformLocation("phong_exp");
}

void MainView::prepareData()
{
    glGenVertexArrays(1, &(meshVAO));
    glGenBuffers(1, &(meshVBO));
    loadMesh(":/models/grid.obj", &meshVAO, &meshVBO);
}

void MainView::loadMesh(QString model_file, GLuint* VAO, GLuint* VBO)
{
    Model model(model_file);
    QVector<QVector3D> vertexCoords = model.getVertices();
    QVector<QVector3D> vertexNormals = model.getNormals();

    QVector<float> meshData;
    meshData.reserve(2 * 3 * vertexCoords.size() + 2 * vertexCoords.size());

    for (int i = 0; i < vertexCoords.length(); i++) {
        QVector3D coord = vertexCoords[i];
        QVector3D normals = vertexNormals[i];
        meshData.append(coord.x());
        meshData.append(coord.y());
        meshData.append(coord.z());
        meshData.append(normals.x());
        meshData.append(normals.y());
        meshData.append(normals.z());
        //add uv coordinates
        meshData.append((coord.x()+1)/2);
        meshData.append((coord.y()+1)/2);
    }

    meshSize = vertexCoords.size();
    // bind VAO
    glBindVertexArray(*VAO);

    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);

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

    paintNormal();
}


void MainView::paintNormal()
{
    shaderProgram_water.bind();

    QMatrix3x3 normal_transforming = meshTransform.normalMatrix();

    //set wave data
    int numberOfWaves = 8;
    float frequency[8] = {6.0, 5.0, 7.0, 8.0, 6.5, 3.0, 1.9, 4.7};
    float amplitude[8] = {0.03, 0.04, 0.002, 0.07, 0.008, 0.013, 0.029, 0.046};
    float phase[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    time += 1.0/60.0;

    // Set uniforms
    glUniformMatrix4fv(uniformProjectionTransform, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransform, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormal_transformation, 1, GL_FALSE, normal_transforming.data());
    glUniform1i(uniformNumberWaves, numberOfWaves);
    glUniform1fv(uniformAmplitude, numberOfWaves, amplitude);
    glUniform1fv(uniformFrequency, numberOfWaves, frequency);
    glUniform1fv(uniformPhase, numberOfWaves, phase);
    glUniform1f(uniformTime, time);

    // Set the lightdata
    QVector3D light_position = QVector3D(10.0, 0.5, 10.0);
    QVector3D light_intensity = QVector3D(1.0, 1.0, 1.0);
    glUniform3f(uniformLightPosition, light_position[0], light_position[1], light_position[2]);
    glUniform3f(uniformLightIntensity, light_intensity[0], light_intensity[1], light_intensity[2]);

    //set material data
    QVector3D material_kd = QVector3D(1.0, 1.0, 1.0);
    QVector3D material_ka = QVector3D(1.0, 1.0, 1.0);
    QVector3D material_ks = QVector3D(1.0, 1.0, 1.0);
    float phong_exp = 32.0;

    glUniform3f(uniformMaterial_ka, material_ka.x(), material_ka.y(), material_ka.z());
    glUniform3f(uniformMaterial_kd, material_kd.x(), material_kd.y(), material_kd.z());
    glUniform3f(uniformMaterial_ks, material_ks.x(), material_ks.y(), material_ks.z());
    glUniform1f(uniformPhongExp, phong_exp);

    // Set the meshdata
    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    shaderProgram_water.release();
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
    meshTransform.translate(0, 0, -5.0);
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

