#-------------------------------------------------
#
# Project created by QtCreator 2013-09-25T09:11:42
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyOpenGL
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    myglwidget.cpp \
    vector3f.cpp \
    particle.cpp \
    matrix34.cpp \
    gl_geometry.cpp \
    random.cpp \
    simulation.cpp \
    particle_force_generator.cpp

HEADERS  += window.h \
    myglwidget.h \
    vector3f.h \
    matrix34.h \
    particle.h \
    simulation.h \
    body.h \
    gl_geometry.h \
    random.h \
    particle_force_generator.h

FORMS    += window.ui
