#-------------------------------------------------
#
# Project created by QtCreator 2016-08-11T19:52:52
#
#-------------------------------------------------
include ( ${PWD}/../qwt.prf )

INCLUDEPATH += X:\Qt\Qt5.7.0.MinGW530\5.7\mingw53_32\include\Qwt

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VirsualOscilloscope
TEMPLATE = app

HEADERS = \
    signaldata.h \
    plot.h \
    knob.h \
    wheelbox.h \
    samplingthread.h \
    curvedata.h \
    mainwindow.h

SOURCES = \
    signaldata.cpp \
    plot.cpp \
    knob.cpp \
    wheelbox.cpp \
    samplingthread.cpp \
    curvedata.cpp \
    mainwindow.cpp \
    main.cpp
