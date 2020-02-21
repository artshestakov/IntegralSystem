#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include "GLLogger.h"

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
		//LG_INFO("testing");
		//return;
		for (int i = 0; i < 500; ++i)
		{
			LG_INFO(std::to_string(i));;
		}
	});
	
	return Application.exec();
}
