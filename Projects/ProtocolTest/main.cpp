#include "PTMainWindow.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication Applcation(argc, argv);

	QString ErrorString;
	if (!ISCore::Startup(true, "ProtocolTest", ErrorString))
	{
		QMessageBox::critical(nullptr, "Error", ErrorString, QMessageBox::Ok);
		return EXIT_FAILURE;
	}

	PTMainWindow MainWindow;
	MainWindow.showMaximized();
	return Applcation.exec();
}
//-----------------------------------------------------------------------------
