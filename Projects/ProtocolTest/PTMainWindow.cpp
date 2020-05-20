#include "PTMainWindow.h"
#include "ISConfig.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
#include "ISTcpQueryAuth.h"
#include "ISConstants.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
PTMainWindow::PTMainWindow(QWidget *parent)
	: QWidget(parent),
	ThreadCarat(new PTThreadCarat(this))
{
	connect(ThreadCarat, &PTThreadCarat::StateChanged, this, &PTMainWindow::ServerStateChanged);

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

	QVBoxLayout *LayoutClient = new QVBoxLayout();

	GroupBoxClient = new QGroupBox("Client", this);
	GroupBoxClient->setLayout(LayoutClient);
	GroupBoxClient->setEnabled(false);

	QHBoxLayout *LayoutClientTop = new QHBoxLayout();
	LayoutClient->addLayout(LayoutClientTop);

	LayoutClientTop->addWidget(new QLabel("Login:", GroupBoxClient));

	EditLogin = new QLineEdit(GroupBoxClient);
	EditLogin->setText(CONFIG_STRING("Client/Login"));
	LayoutClientTop->addWidget(EditLogin);

	LayoutClientTop->addWidget(new QLabel("Password:", GroupBoxClient));

	EditPassword = new QLineEdit(GroupBoxClient);
	EditPassword->setText(CONFIG_STRING("Client/Password"));
	LayoutClientTop->addWidget(EditPassword);

	ButtonConnect = new QPushButton("Connect", GroupBoxClient);
	connect(ButtonConnect, &QPushButton::clicked, this, &PTMainWindow::Connect);
	LayoutClientTop->addWidget(ButtonConnect);

	ButtonDisconnect = new QPushButton("Disconnect", GroupBoxClient);
	ButtonDisconnect->setEnabled(false);
	connect(ButtonDisconnect, &QPushButton::clicked, this, &PTMainWindow::Disconnect);
	LayoutClientTop->addWidget(ButtonDisconnect);

	LayoutClientTop->addStretch();

	QHBoxLayout *LayoutQuery = new QHBoxLayout();
	LayoutClient->addLayout(LayoutQuery);

	LayoutQuery->addWidget(new QLabel("Query:", GroupBoxClient));

	ComboBox = new QComboBox(GroupBoxClient);
	ComboBox->setEnabled(false);
	ComboBox->addItem(API_TEST_QUERY, API_TEST_QUERY);
	ComboBox->addItem(API_SLEEP, API_SLEEP);
	ComboBox->addItem(API_COLUMN_SIZER, API_COLUMN_SIZER);
	ComboBox->setCurrentText(CONFIG_STRING("Client/LastQuery"));
	LayoutQuery->addWidget(ComboBox);

	ButtonSend = new QPushButton("Send", GroupBoxClient);
	ButtonSend->setEnabled(false);
	connect(ButtonSend, &QPushButton::clicked, this, &PTMainWindow::Send);
	LayoutQuery->addWidget(ButtonSend);

	LayoutQuery->addStretch();

	TextEdit = new QTextEdit(GroupBoxClient);
	TextEdit->setReadOnly(true);
	LayoutClient->addWidget(TextEdit);

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
void PTMainWindow::closeEvent(QCloseEvent *CloseEvent)
{
	bool Result = ThreadCarat->isRunning();
	if (Result)
	{
		Result = QMessageBox::question(this, "Exit", "Stop listen server?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes;
		if (Result)
		{
			QWidget::closeEvent(CloseEvent);
		}
		else
		{
			CloseEvent->ignore();
		}
	}
	else
	{
		QWidget::closeEvent(CloseEvent);
	}
}
//-----------------------------------------------------------------------------
void PTMainWindow::Listen()
{
	int Port = EditListenPort->value();
	ISConfig::Instance().SetValue("Server/Port", Port);
	ThreadCarat->Start(Port);
}
//-----------------------------------------------------------------------------
void PTMainWindow::Unlisten()
{
	ThreadCarat->quit();
	EditListenPort->setEnabled(true);
	ButtonListen->setEnabled(true);
	ButtonUnlisten->setEnabled(false);
	GroupBoxClient->setEnabled(false);
}
//-----------------------------------------------------------------------------
void PTMainWindow::Connect()
{
	QString Login = EditLogin->text();
	QString Password = EditPassword->text();

	ISConfig::Instance().SetValue("Client/Login", Login);
	ISConfig::Instance().SetValue("Client/Password", Password);
	bool Result = ISTcpConnector::Instance().Connect("127.0.0.1", EditListenPort->value(), Login, Password);
	if (Result)
	{
		ISTcpQueryAuth qAuth(Login, Password);
		Result = qAuth.Execute();
		if (Result)
		{
			Result = ISTcpConnector::Instance().Reconnect("127.0.0.1", qAuth.GetAnswer()["Port"].toInt(), Login, Password);
			if (!Result)
			{
				QMessageBox::critical(this, "Error", ISTcpConnector::Instance().GetErrorString());
			}
		}
		else
		{
			QMessageBox::critical(this, "Error", qAuth.GetErrorString());
		}
	}
	else
	{
		QMessageBox::critical(this, "Error", ISTcpConnector::Instance().GetErrorString());
	}

	ButtonConnect->setEnabled(!Result);
	ButtonDisconnect->setEnabled(Result);
	ComboBox->setEnabled(Result);
	ButtonSend->setEnabled(Result);
}
//-----------------------------------------------------------------------------
void PTMainWindow::Disconnect()
{
	ISTcpConnector::Instance().Disconnect();
	ButtonConnect->setEnabled(true);
	ButtonDisconnect->setEnabled(false);
	ComboBox->setEnabled(false);
	ButtonSend->setEnabled(false);
}
//-----------------------------------------------------------------------------
void PTMainWindow::Send()
{
	QString QueryType = ComboBox->currentData().toString();
	ISConfig::Instance().SetValue("Client/LastQuery", QueryType);
	ISTcpQuery qQuery(QueryType);
	ISCountingTime CountingTime;
	bool Result = qQuery.Execute();
	if (Result)
	{
		TextEdit->append(ISSystem::VariantMapToJsonString(qQuery.GetAnswer()));
		TextEdit->append("Query OK: " + QString::number(CountingTime.Elapsed()) + " msec");
	}
	else
	{
		QMessageBox::critical(this, "Error", qQuery.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void PTMainWindow::ServerStateChanged(bool Running)
{
	if (Running)
	{
		EditListenPort->setEnabled(!Running);
		ButtonListen->setEnabled(!Running);
		ButtonUnlisten->setEnabled(Running);
		GroupBoxClient->setEnabled(Running);
	}
	else
	{
		QMessageBox::critical(this, "Error", ThreadCarat->GetErrorString());
	}
}
//-----------------------------------------------------------------------------
