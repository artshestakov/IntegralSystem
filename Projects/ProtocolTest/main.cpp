
#include "PTMainWindow.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication Applcation(argc, argv);
	PTMainWindow MainWindow;
	MainWindow.show();
	return Applcation.exec();
}
//-----------------------------------------------------------------------------
