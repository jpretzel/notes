# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

symbian:TARGET.UID3 = 0xE0798E68

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
CONFIG += mobility
MOBILITY += systeminfo \
            messaging \
            contacts

INCLUDEPATH += /Users/sebastian/Downloads/qt-mobility-opensource-src-1.2.0/src/messaging

symbian:TARGET.CAPABILITY = NetworkServices \
        LocalServices \
        ReadUserData \
        WriteUserData \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData

SOURCES += main.cpp \
            mainwindow.cpp \
            notebutton.cpp \
            notepainterwidget.cpp

HEADERS += mainwindow.h \
            notebutton.h \
            notepainterwidget.h

FORMS += mainwindow.ui \
            notepainterwidget.ui

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

RESOURCES += \
    images.qrc

