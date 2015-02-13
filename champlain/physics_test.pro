#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T23:32:44
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_physics_testtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_physics_testtest.cpp \
    ../matrix34.cpp \
    ../particle.cpp \
    ../particle_contact.cpp \
    ../particle_contact_generator.cpp \
    ../particle_force_generator.cpp \
    ../particle_link.cpp \
    ../particle_system.cpp \
    ../random.cpp \
    ../simulation.cpp \
    ../vector3f.cpp \
    quaternion.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../body.h \
    ../matrix34.h \
    ../particle.h \
    ../particle_contact.h \
    ../particle_contact_generator.h \
    ../particle_force_generator.h \
    ../particle_link.h \
    ../particle_system.h \
    ../random.h \
    ../simulation.h \
    ../vector3f.h \
    quaternion.h
