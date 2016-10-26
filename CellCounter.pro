QT       += core gui xml widgets
QT       -= test
qtHaveModule(printsupport): QT += printsupport
TARGET = CellCounter
TEMPLATE = app
#FOR WINDOWS
#win32:INCLUDEPATH += C:\opencv243\opencv\build\include
#win32:LIBS += C:\opencv243\opencv\build\x86\mingw\lib\libopencv_core243.dll.a
#win32:LIBS += C:\opencv243\opencv\build\x86\mingw\lib\libopencv_highgui243.dll.a
#win32:LIBS += C:\opencv243\opencv\build\x86\mingw\lib\libopencv_imgproc243.dll.a
#win32:LIBS += C:\opencv243\opencv\build\x86\mingw\lib\libopencv_legacy243.dll.a
#FOR MAC
macx: INCLUDEPATH += /usr/local/include
macx: LIBS += /usr/local/lib/libopencv_core.2.4.9.dylib
macx: LIBS += /usr/local/lib/libopencv_highgui.2.4.9.dylib
macx: LIBS += /usr/local/lib/libopencv_legacy.2.4.9.dylib
macx: LIBS += /usr/local/lib/libopencv_imgproc.2.4.9.dylib

#macx:DEPENDPATH += $$PWD/../../../../opt/local/include
#FOR LINUX
#unix:INCLUDEPATH += /usr/local/include/opencv
#unix:LIBS += /usr/local/lib/libopencv_core.so.2.4.3
#unix:LIBS += /usr/local/lib/libopencv_highgui.so.2.4.3
#unix:LIBS += /usr/local/lib/libopencv_imgproc.so.2.4.3
#unix:LIBS += /usr/local/lib/libopencv_legacy.so.2.4.3

SOURCES += main.cpp\
    csopencv.cpp \
    csmorphology.cpp \
    csthreshold.cpp \
    csparameter.cpp \
    cellcounter.cpp \
    csview.cpp \
    csnode.cpp \
    xccelgenerator.cpp \
    xccelhandler.cpp

HEADERS  += \
    csopencv.h \
    csmorphology.h \
    csthreshold.h \
    csparameter.h \
    cellcounter.h \
    csview.h \
    csnode.h \
    xccelgenerator.h \
    xccelhandler.h

FORMS    += \
    cellcounter.ui

RESOURCES += \
    icons.qrc

OTHER_FILES += \
    cellcounter.rc \
    cellcounter.desktop \
    cellcounter.icns
win32:RC_FILE = cellcounter.rc
macx:ICON = cellcounter.icns


