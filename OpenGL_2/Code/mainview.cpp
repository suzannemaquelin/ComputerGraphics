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

    glDeleteBuffers(1, &VBO_cat);
    glDeleteVertexArrays(1, &VAO_cat);
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

    createCat();
    matrixInit();
}

void MainView::createCat()
{
    catVertices = (Model(":/models/cat.obj")).getVertices();
    catNormals = (Model(":/models/cat.obj")).getNormals();

    Vertex s[catVertices.length()];

    for (int i = 0; i < catVertices.length(); i++) {
        s[i] = Vertex(catVertices[i].x(), catVertices[i].y(), catVertices[i].z(), catNormals[i].x(), catNormals[i].y(), catNormals[i].z());
    }
    glGenVertexArrays(1,&VAO_cat);
    glBindVertexArray(VAO_cat);
    glGenBuffers(1,&VBO_cat);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_cat);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));

    glBufferData(GL_ARRAY_BUFFER,catVertices.length()*sizeof(Vertex),s,GL_STATIC_DRAW);
//    free(s);
}

void MainView::matrixInit()
{
// Translations
    translation_cat = QMatrix4x4();
    translation_cat.translate(QVector3D(0,0,-2));

// Rotation/scale
    rotation = QMatrix4x4();
    scale = QMatrix4x4();
    rotation.setToIdentity();
    scale.setToIdentity();

// Perspective
    perspective = QMatrix4x4();
    aspect_ratio = (float) this->width()/ (float) this->height();
    perspective.perspective(60, aspect_ratio, 0.01, 1000);
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

    QMatrix3x3 normal_transforming = rotation.normalMatrix();
    normal_transforming.setToIdentity();

    shaderProgram.bind();

    glUniformMatrix4fv(shaderProgram.uniformLocation("perspective"), 1, GL_FALSE, perspective.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("rotation"), 1, GL_FALSE, rotation.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("scale"), 1, GL_FALSE, scale.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("x"), 1, GL_FALSE, normal_transforming.data());

    //Draw cat here
    glUniformMatrix4fv(shaderProgram.uniformLocation("translation"), 1, GL_FALSE, translation_cat.data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("normal_transformation"), 1, GL_FALSE, normal_transforming.data());
    glBindVertexArray(VAO_cat);
    glDrawArrays(GL_TRIANGLES,0,catVertices.length());

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
    aspect_ratio = (float) newWidth / (float) newHeight;
    perspective.setToIdentity();
    perspective.perspective(60, aspect_ratio, 0.01, 1000);
    update();
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
    scale.setToIdentity();
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
