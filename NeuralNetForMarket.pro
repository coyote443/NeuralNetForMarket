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
    mainWindow.cpp \
    neuralNetwork.cpp

HEADERS  += \
    neuron.h \
    teacher.h \
    headerNeuron.h \
    mainWindow.h \
    neuralNetwork.h \
    headerMain.h

FORMS    += mainwindow.ui
