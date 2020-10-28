#include "LVMainWindow.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication Application(argc, argv);
	LVMainWindow MainWindow;
	MainWindow.showMaximized();
    return Application.exec();
}
//-----------------------------------------------------------------------------
