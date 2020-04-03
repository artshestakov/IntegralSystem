#include "GAMainWindow.h"
#include "ASLogger.h"
//-----------------------------------------------------------------------------
GAMainWindow::GAMainWindow(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QPushButton *ButtonStart = new QPushButton("Start", this);
	connect(ButtonStart, &QPushButton::clicked, this, &GAMainWindow::Start);
	MainLayout->addWidget(ButtonStart);

	QPushButton *ButtonLog = new QPushButton("Log", this);
	connect(ButtonLog, &QPushButton::clicked, this, &GAMainWindow::Log);
	MainLayout->addWidget(ButtonLog);

	QPushButton *ButttonDestroy = new QPushButton("Destroy", this);
	connect(ButttonDestroy, &QPushButton::clicked, this, &GAMainWindow::Destroy);
	MainLayout->addWidget(ButttonDestroy);
}
//-----------------------------------------------------------------------------
GAMainWindow::~GAMainWindow()
{

}
//-----------------------------------------------------------------------------
void GAMainWindow::Start()
{
	ASLogger::Instance().Initialize();
}
//-----------------------------------------------------------------------------
void GAMainWindow::Log()
{
	ASLOGGER_D("test");
}
//-----------------------------------------------------------------------------
void GAMainWindow::Destroy()
{
	close();
}
//-----------------------------------------------------------------------------
