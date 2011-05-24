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

#INCLUDEPATH += /Users/sebastian/Downloads/qt-mobility-opensource-src-1.2.0/src/messaging

symbian:TARGET.CAPABILITY = NetworkServices \
        LocalServices \
        ReadUserData \
        WriteUserData \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData

symbian:LIBS += -lcone -leikcore -lavkon

SOURCES += main.cpp \
            mainwindow.cpp \
            notebutton.cpp \
            notepainterwidget.cpp \
            sendnotedialog.cpp \
            helpwidget.cpp

HEADERS += mainwindow.h \
            notebutton.h \
            notepainterwidget.h \
            sendnotedialog.h \
            helpwidget.h

FORMS += mainwindow.ui \
            notepainterwidget.ui \
            sendnotedialog.ui

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

RESOURCES += \
    images.qrc

