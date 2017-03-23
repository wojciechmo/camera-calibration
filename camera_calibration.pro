TEMPLATE = app
#CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
INCLUDEPATH += /usr/local/lib
LIBS += -L"/usr/local/lib"
LIBS += -lopencv_imgcodecs -lopencv_aruco -lopencv_core -lopencv_photo -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_videoio -lopencv_features2d -lopencv_flann -lopencv_calib3d -lopencv_viz




