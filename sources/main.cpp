//#include <QWSServer>
#include <QApplication>

#include "linbogui.h"

int main( int argc, char* argv[] )
{
  QApplication linboGuiApp( argc, argv );
  /*QWSServer* wsServer = QWSServer::instance();

  if ( wsServer ) {
    wsServer->refresh();
  }*/

  qDebug() << TEST_ENV;

  LinboGui* linboGui = new LinboGui;
  linboGui->show();

  return linboGuiApp.exec();
}
