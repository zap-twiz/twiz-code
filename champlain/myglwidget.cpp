// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>

#include "myglwidget.h"
#include "simulation.h"

MyGLWidget::MyGLWidget(QWidget *parent)
    : //QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
      QGLWidget(parent) {
  xRot = 0;
  yRot = 0;
  zRot = 0;

  simulation_ = 0;
}

MyGLWidget::~MyGLWidget()
{
  delete simulation_;
  simulation_ = NULL;
}

QSize MyGLWidget::minimumSizeHint() const
{
  return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
{
  return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void MyGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        emit updateGL();
    }
}

void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        emit updateGL();
    }
}

void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        emit updateGL();
    }
}

void MyGLWidget::initializeGL()
{

//  simulation_ = new FireworksSimulation();
//  simulation_ = new MultiRodSimulation();
//  simulation_ = new SimpleGravitySimulation();
  simulation_ = new TetrahedronSimulation();
  simulation_->Reset();


    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -4.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    draw();
}

void MyGLWidget::onTick()
{
//    emit setYRotation(yRot + 1);
//    emit setXRotation(xRot + 1);
//    emit setZRotation(zRot + 1);
  //emit updateGL();

  for (int x= 0; x < 16; ++x) {
    simulation_->Step(0.001);
  }

  //paintGL();
}

void MyGLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#if 0
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
#endif
    glFrustum(-2, +2, -2, +2, 1.0, 5000.0);
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }

    lastPos = event->pos();
}

void drawCube()
{
    glBegin(GL_QUADS);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);
    glVertex3f(-1.0f, 1.0f,-1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);
    glColor3f(1.0f,0.5f,0.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glColor3f(1.0f,1.0f,0.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f, 1.0f,-1.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f,-1.0f);
    glVertex3f(-1.0f,-1.0f, 1.0f);
    glColor3f(1.0f,0.0f,1.0f);
    glVertex3f( 1.0f, 1.0f,-1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f,-1.0f);
    glEnd();
}

#include "gl_geometry.h"

void MyGLWidget::draw()
{    

    glPushMatrix();
    Q_ASSERT(GL_NO_ERROR == glGetError());
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glEnable(GL_COLOR_MATERIAL);


    glDisable(GL_LIGHTING);
    // prototypeing work for display of forces ....
    glBegin(GL_LINES);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    glBegin(GL_LINES);
      glColor3f(0.0f, 1.0f, 0.0f);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    glBegin(GL_LINES);
      glColor3f(0.0f, 0.0f, 1.0f);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    glEnable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);

    ParticleSystem& system = simulation_->getSystem();

    std::vector<Particle>& particles = system.particles();
    for ( Particle const & particle : particles ) {
      glPushMatrix();
      Vector3f position = particle.position();
      glTranslatef(position.x(), position.y(), position.z());

      //std::cout << "Position y " << position.y() << std::endl;
      GLSphere sphere(0.25);
      sphere.draw();
      glPopMatrix();
    }

    glPopMatrix();
    return;
}
