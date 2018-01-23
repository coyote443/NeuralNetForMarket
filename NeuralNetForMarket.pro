#-------------------------------------------------
#
# Project created by QtCreator 2018-01-05T13:14:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NeuralNetForMarket
TEMPLATE = app

CONFIG += console c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    neuron.cpp \
    neuralnetwork.cpp \
    teacher.cpp

HEADERS  += mainwindow.h \
    neuron.h \
    header.h \
    neuralnetwork.h \
    teacher.h \
    headermain.h

FORMS    += mainwindow.ui
