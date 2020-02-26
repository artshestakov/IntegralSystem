#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QInputDialog>
#include "GLLogger.h"
#include <windows.h>

int main(int argc, char *argv[])
{
	QApplication Application(argc, argv);
	
	GLLogger::Instance().Initialize();

	QWidget Widget;
	Widget.setLayout(new QVBoxLayout());
	Widget.show();
	
	QPushButton Button(&Widget);
	Widget.layout()->addWidget(&Button);
	QObject::connect(&Button, &QPushButton::clicked, [=]
	{
		int Size = QInputDialog::getInt(nullptr, QString(), QString());
		for (int i = 0; i < Size; ++i)
		{
			LG_INFO(std::to_string(i));
			//Sleep(10);
		}
	});
	
	//LG_INFO(std::to_string(1));
	return Application.exec();
}
