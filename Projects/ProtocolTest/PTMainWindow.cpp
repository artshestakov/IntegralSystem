#include "PTMainWindow.h"
#include "ISConfig.h"
#include "ISTcpConnector.h"
//-----------------------------------------------------------------------------
PTMainWindow::PTMainWindow(QWidget *parent)
	: QWidget(parent),
	Server(new ISTcpServerCarat(this))
{
	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QHBoxLayout *LayoutServer = new QHBoxLayout();

	QGroupBox *GroupBoxServer = new QGroupBox("Server", this);
	GroupBoxServer->setLayout(LayoutServer);
	GroupBoxServer->setSizePolicy(GroupBoxServer->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	MainLayout->addWidget(GroupBoxServer);

	LayoutServer->addWidget(new QLabel("Listen Port:", GroupBoxServer));

	EditListenPort = new QSpinBox(GroupBoxServer);
	EditListenPort->setRange(0, USHRT_MAX);
	EditListenPort->setValue(CONFIG_INT("Server/Port"));
	LayoutServer->addWidget(EditListenPort);

	ButtonListen = new QPushButton("Listen", GroupBoxServer);
	connect(ButtonListen, &QPushButton::clicked, this, &PTMainWindow::Listen);
	LayoutServer->addWidget(ButtonListen);

	ButtonUnlisten = new QPushButton("Unlisten", GroupBoxServer);
	ButtonUnlisten->setEnabled(false);
	connect(ButtonUnlisten, &QPushButton::clicked, this, &PTMainWindow::Unlisten);
	LayoutServer->addWidget(ButtonUnlisten);

	QCheckBox *CheckAutoListen = new QCheckBox("Auto listen", GroupBoxServer);
	CheckAutoListen->setChecked(CONFIG_BOOL("Server/AutoListen"));
	connect(CheckAutoListen, &QCheckBox::stateChanged, [=] (int NewState)
	{
		ISConfig::Instance().SetValue("Server/AutoListen", NewState == Qt::Checked);
	});
	LayoutServer->addWidget(CheckAutoListen);

	LayoutServer->addStretch();

	QGroupBox *GroupBoxClient = new QGroupBox("Client", this);
	MainLayout->addWidget(GroupBoxClient);

	if (CheckAutoListen->isChecked())
	{
		Listen();
	}
}
//-----------------------------------------------------------------------------
PTMainWindow::~PTMainWindow()
{

}
//-----------------------------------------------------------------------------
void PTMainWindow::Listen()
{
	int Port = EditListenPort->value();
	ISConfig::Instance().SetValue("Server/Port", Port);
	bool Result = Server->Run(Port);
	if (Result)
	{
		EditListenPort->setEnabled(!Result);
		ButtonListen->setEnabled(!Result);
		ButtonUnlisten->setEnabled(Result);
	}
	else
	{
		QMessageBox::critical(this, "Error", Server->GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void PTMainWindow::Unlisten()
{
	Server->close();
	EditListenPort->setEnabled(true);
	ButtonListen->setEnabled(true);
	ButtonUnlisten->setEnabled(false);
}
//-----------------------------------------------------------------------------
