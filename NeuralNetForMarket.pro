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
    neuron.cpp \
    teacher.cpp \
    neuralnetwork.cpp \
    mainwindow.cpp

HEADERS  += \
    neuron.h \
    teacher.h \
    neuralnetwork.h \
    mainwindow.h \
    headerneuron.h \
    headermain.h

FORMS    += mainwindow.ui
