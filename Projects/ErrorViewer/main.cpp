#include <QtWidgets/QApplication>
#include "EVMainWindow.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		return EXIT_FAILURE;
	}

	QApplication Application(argc, argv);
	EVMainWindow MainWindow(argv[1]);
	MainWindow.show();
	return Application.exec();
}
//-----------------------------------------------------------------------------
