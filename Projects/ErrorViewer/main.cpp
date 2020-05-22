#include <QtWidgets/QApplication>
#include "EVMainWindow.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	QApplication Application(argc, argv);
	EVMainWindow MainWindow(argc == 2 ? argv[1] : QString());
	MainWindow.show();
	return Application.exec();
}
//-----------------------------------------------------------------------------
