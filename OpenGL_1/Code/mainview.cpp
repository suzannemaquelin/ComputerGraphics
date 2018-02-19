#include "mainview.h"
#include "math.h"
#include "vertex.h"

#include <cstdlib>
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
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";


    glDeleteBuffers(1, &VBO_cube);
    glDeleteBuffers(1, &VBO_pyramid);
    glDeleteBuffers(1, &VBO_sphere);
    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteVertexArrays(1, &VAO_pyramid);
    glDeleteVertexArrays(1, &VAO_sphere);

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

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color of the screen to be black on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);


    //shaderProgram = new QOpenGLShaderProgram();
    createShaderProgram();

    createCube();
    createPyramid();
    createSphere();
    matrixInit();
}

void MainView::createSphere()
{
    sphere = (Model(":/models/sphere.obj")).getVertices();

    Vertex s[sphere.length()];

    for (int i = 0; i < sphere.length(); i++) {
        s[i] = Vertex(sphere[i].x(), sphere[i].y(), sphere[i].z(),rand()%2,rand()%2,rand()%2);
    }
    glGenVertexArrays(1,&VAO_sphere);
    glBindVertexArray(VAO_sphere);
    glGenBuffers(1,&VBO_sphere);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_sphere);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER,sphere.length()*sizeof(Vertex),s,GL_STATIC_DRAW);
//    free(s);
}

void MainView::createCube()
{
    glGenVertexArrays(1,&VAO_cube);
    glBindVertexArray(VAO_cube);
    glGenBuffers(1,&VBO_cube);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_cube);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));

    Vertex v1 = Vertex(-1, -1, -1, 1, 0, 0);
    Vertex v2 = Vertex(-1, -1, 1, 0, 1, 0);
    Vertex v5 = Vertex(-1, 1, -1, 1, 0, 0);
    Vertex v6 = Vertex(-1, 1, 1, 0, 0, 1);
    Vertex v4 = Vertex(1, -1, -1, 0, 1, 0);
    Vertex v3 = Vertex(1, -1, 1, 0, 0, 1);
    Vertex v8 = Vertex(1, 1, -1, 0, 0, 1);
    Vertex v7 = Vertex(1, 1, 1, 1, 0, 0);

    Vertex bottom[6] = {v4, v3, v1, v2, v1, v3};
    Vertex front[6] = {v2, v3, v6, v3, v7, v6};
    Vertex left[6] = {v6, v5, v2, v5, v1, v2};
    Vertex right[6] = {v8, v7, v3, v3, v4, v8};
    Vertex rear[6] = {v1, v5, v8, v8, v4, v1};
    Vertex top[6] = {v5, v6, v8, v6, v7, v8};
    Vertex cube[36] = {v4, v3, v1, v2, v1, v3,v2, v3, v6, v3, v7, v6,v6, v5, v2, v5, v1, v2,v8, v7, v3, v3, v4, v8,v1, v5, v8, v8, v4, v1,v5, v6, v8, v6, v7, v8};

    glBufferData(GL_ARRAY_BUFFER,36*sizeof(Vertex),cube,GL_STATIC_DRAW);

}

void MainView::createPyramid()
{
    glGenVertexArrays(1,&VAO_pyramid);
    glBindVertexArray(VAO_pyramid);
    glGenBuffers(1,&VBO_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_pyramid);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));


    //create vertice for pyramid
    Vertex p9 = Vertex(0,1,0, 1, 0, 0);
    Vertex p1 = Vertex(-1, -1, -1, 0, 0, 1);
    Vertex p2 = Vertex(-1, -1, 1, 0, 0, 1);
    Vertex p4 = Vertex(1, -1, -1, 0, 1, 0);
    Vertex p3 = Vertex(1, -1, 1, 0, 1, 0);

    Vertex pbottom[6] = {p4, p3, p1, p2, p1, p3};
    Vertex pfront[3] = {p9, p2, p3};
    Vertex prear[3] = {p9, p4, p1};
    Vertex pleft[3] = {p9, p1, p2};
    Vertex pright[3] = {p9, p3, p4};
    Vertex pyramid[18] = {p4, p3, p1, p2, p1, p3,p9, p2, p3,p9, p4, p1,p9, p1, p2,p9, p3, p4};


    glBufferData(GL_ARRAY_BUFFER,18*sizeof(Vertex),pyramid,GL_STATIC_DRAW);

}

void MainView::matrixInit()
{
// Translations
    // Cube to 2,0,−6
    translation_cube = QMatrix4x4();
//    translation_cube.translate(QVector3D(2,0,-6));
    translation_cube.translate(QVector3D(0,0,0));
    // Pyramid to -2,0,−6
    translation_pyramid = QMatrix4x4();
//    translation_pyramid.translate(QVector3D(-2,0,-6));
    translation_pyramid.translate(QVector3D(0,0,0));
    // Sphere to 0,0,−10
    translation_sphere = QMatrix4x4();
//    translation_sphere.translate(QVector3D(0,0,-10));
    translation_sphere.translate(QVector3D(0,0,0));

// Rotation/scale
    rotation = QMatrix4x4();
    scale = QMatrix4x4();

// Perspective
    perspective = QMatrix4x4();
    perspective.perspective(60, aspect_ratio, 1, -1);
}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

}

// --- OpenGL drawing

/**default_scale
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glUniformMatrix4fv(shaderProgram.uniformLocation("perspective"), 1, GL_FALSE, perspective.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("rotation"), 1, GL_FALSE, rotation.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("scale"), 1, GL_FALSE, scale.data());
    QMatrix4x4 sphereScale = QMatrix4x4(scale);
    sphereScale.scale(0.04);

//    glUniformMatrix4fv(shaderProgram.uniformLocation("original_scale"), 1, GL_FALSE, default_scale.data());

    // Draw cube here
    glUniformMatrix4fv(shaderProgram.uniformLocation("translation"), 1, GL_FALSE, translation_cube.data());
    glBindVertexArray(VAO_cube);
    glDrawArrays(GL_TRIANGLES,0,36);

    // Draw pyramid here
    glUniformMatrix4fv(shaderProgram.uniformLocation("translation"), 1, GL_FALSE, translation_pyramid.data());
    glBindVertexArray(VAO_pyramid);
    glDrawArrays(GL_TRIANGLES,0,18);

    //Draw sphere here
    glUniformMatrix4fv(shaderProgram.uniformLocation("translation"), 1, GL_FALSE, translation_sphere.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("scale"), 1, GL_FALSE, sphereScale.data());
    glBindVertexArray(VAO_sphere);
    glDrawArrays(GL_TRIANGLES,0,sphere.length());


    shaderProgram.release();
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
    // TODO: Update projection to fit the new aspect ratio
//    Q_UNUSED(newWidth)
//    Q_UNUSED(newHeight)
    aspect_ratio = newWidth / (float) newHeight;
    //perspective.perspective(60, aspect_ratio, 1, -1);
    //update();
//    perspective = QMatrix4x4();
//    perspective.perspective();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    //convert degrees to radians
    float rX = (rotateX/360.0)*(2*M_PI);
    float rY = (rotateY/360.0)*(2*M_PI);
    float rZ = (rotateZ/360.0)*(2*M_PI);

    QMatrix4x4 rotationX = QMatrix4x4(
                1, 0, 0, 0,
                0, cos(rX), -sin(rX), 0,
                0, sin(rX), cos(rX), 0,
                0, 0, 0, 1);

    QMatrix4x4 rotationY = QMatrix4x4(
                cos(rY), 0, sin(rY), 0,
                0, 1, 0, 0,
                -sin(rY), 0, cos(rY), 0,
                0, 0, 0, 1);

    QMatrix4x4 rotationZ = QMatrix4x4(
                cos(rZ), -sin(rZ), 0, 0,
                sin(rZ), cos(rZ), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
    rotation = rotationX *rotationY * rotationZ;
    update();
}

void MainView::setScale(int scaleIn)
{
    //convert scale to value between 0 and 2
    float s = scaleIn/100.0;

    scale = QMatrix4x4();
    scale.scale(s);
    update();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
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
