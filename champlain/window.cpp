// window.cpp

#include <QtWidgets>
#include "window.h"
#include "ui_window.h"

#include "myglwidget.h"

#include <qtimer.h>

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

//   connect(ui->myGLWidget, SIGNAL(xRotationChanged(int)), ui->rotXSlider, SLOT(setValue(int)));
//   connect(ui->myGLWidget, SIGNAL(yRotationChanged(int)), ui->rotYSlider, SLOT(setValue(int)));
//   connect(ui->myGLWidget, SIGNAL(zRotationChanged(int)), ui->rotZSlider, SLOT(setValue(int)));

    connect(ui->singleStepButton, SIGNAL(clicked()), this, SLOT(timeOutSlot()));

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    m_timer = new QTimer( this );
    //connect( m_timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()) );
    //m_timer->start( 16 );
}

Window::~Window()
{
  m_timer->stop();
  delete ui;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void Window::timeOut()
{
    // do something interesting!?
    ui->myGLWidget->onTick();
    emit ui->myGLWidget->updateGL();
}

void Window::timeOutSlot()
{
    timeOut();
}
