#include <QtWidgets/QApplication>
#include "GAMainWindow.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication Application(argc, argv);
	GAMainWindow MainWindow;
	MainWindow.show();

	int ReturnCode = Application.exec();
	return ReturnCode;
}
//-----------------------------------------------------------------------------
