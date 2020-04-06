#include "GAMainWindow.h"
#include "ASLogger.h"
//-----------------------------------------------------------------------------
GAMainWindow::GAMainWindow(QWidget *parent)
	: QWidget(parent),
	LastValue(0)
{
	resize(500, 500);

	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QPushButton *ButtonStart = new QPushButton("Start", this);
	connect(ButtonStart, &QPushButton::clicked, this, &GAMainWindow::Start);
	MainLayout->addWidget(ButtonStart);

	QPushButton *ButtonLog = new QPushButton("Log", this);
	connect(ButtonLog, &QPushButton::clicked, this, &GAMainWindow::Log);
	MainLayout->addWidget(ButtonLog);

	QPushButton *ButtonLog2 = new QPushButton("Log 2", this);
	connect(ButtonLog2, &QPushButton::clicked, this, &GAMainWindow::Log2);
	MainLayout->addWidget(ButtonLog2);

	QPushButton *ButtonLogN = new QPushButton("Log N", this);
	connect(ButtonLogN, &QPushButton::clicked, this, &GAMainWindow::LogN);
	MainLayout->addWidget(ButtonLogN);

	QPushButton *ButttonDestroy = new QPushButton("Destroy", this);
	connect(ButttonDestroy, &QPushButton::clicked, this, &GAMainWindow::Destroy);
	MainLayout->addWidget(ButttonDestroy);

	MainLayout->addStretch();

	Start();

	Timer = new QTimer(this);
	Timer->setSingleShot(true);
	Timer->setInterval(500);
	connect(Timer, &QTimer::timeout, this, &GAMainWindow::Timeout);
	Timer->start();
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
void GAMainWindow::Log2()
{
	ASLOGGER_D("test 1");
	ASLOGGER_D("test 2");
}
//-----------------------------------------------------------------------------
void GAMainWindow::LogN()
{
	bool Ok = true;
	int Value = QInputDialog::getInt(this, "Input", "Enter count messages:", LastValue, 0, INT_MAX, 1, &Ok);
	if (Ok && Value)
	{
		for (int i = 0; i < Value; ++i)
		{
			ASLOGGER_I(QUuid::createUuid().toString().toStdString());
		}
		LastValue = Value;
	}
}
//-----------------------------------------------------------------------------
void GAMainWindow::Destroy()
{
	ASLogger::Instance().Shutdown();
}
//-----------------------------------------------------------------------------
void GAMainWindow::Timeout()
{
	ASLOGGER_D("test");
	Timer->start();
}
//-----------------------------------------------------------------------------
