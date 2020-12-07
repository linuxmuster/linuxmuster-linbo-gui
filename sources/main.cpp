#include <QApplication>

#include "linbogui.h"

int main( int argc, char* argv[] )
{
  QApplication linboGuiApp( argc, argv );

  QApplication::setStyle("fusion");

  LinboGui* linboGui = new LinboGui;
  linboGui->show();

  return linboGuiApp.exec();
}
