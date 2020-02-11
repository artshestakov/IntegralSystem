#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include "MyLineEdit.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	QWidget Widget;
	Widget.setLayout(new QVBoxLayout());
	Widget.layout()->addWidget(new MyLineEdit(&Widget));
	Widget.show();

	return a.exec();
}
