#include "mainwindow.h"

#include <QtGui/QApplication>

// Needed Symbian specific headers
#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

    // Lock Orientation
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAPD(error,
          if (appUi) {
          // Lock application orientation into landscape
          appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);

}
);
#endif

mainWindow.showExpanded();

return app.exec();
}
