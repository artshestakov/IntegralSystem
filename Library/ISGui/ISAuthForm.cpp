#include "ISAuthForm.h"
#include "ISDefinesGui.h"
#include "ISCore.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISAboutForm.h"
#include "ISConfig.h"
#include "ISConnectionForm.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISAuthForm::ISAuthForm()
	: ISInterfaceDialogForm(),
	ConnectingState(false)
{
	setWindowTitle(LANG("InputInSystem"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QLabel *LableImage = new QLabel(this);
	LableImage->setScaledContents(true);
	LableImage->setPixmap(BUFFER_PIXMAPS("BannerLogoAuth"));
	GetMainLayout()->addWidget(LableImage);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(Layout);

	Layout->addWidget(new QLabel(LANG("InputLoginAndPassword") + ':', this));

	EditLogin = new ISLineEdit(this);
	EditLogin->SetPlaceholderText(LANG("Login"));
	EditLogin->SetIcon(BUFFER_ICONS("Auth.Login"));
	EditLogin->SetRegExp(REG_EXP_LOGIN);
	Layout->addWidget(EditLogin);

	EditPassword = new ISPasswordEdit(this);
	EditPassword->SetPlaceholderText(LANG("Password"));
	EditPassword->SetVisibleGenerate(false);
	EditPassword->SetIcon(BUFFER_ICONS("Auth.Password"));
	EditPassword->SetVisibleCheckBox(false);
	Layout->addWidget(EditPassword);

	QHBoxLayout *LayoutLabels = new QHBoxLayout();
	LayoutLabels->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->addLayout(LayoutLabels);

	CheckRememberUser = new ISCheckEdit(this);
	CheckRememberUser->SetText(LANG("RememberMe"));
	CheckRememberUser->SetValue(CONFIG_BOOL(CONST_CONFIG_REMEMBER_USER_INCLUDE));
	LayoutLabels->addWidget(CheckRememberUser);

	LabelCapsLook = new QLabel(this);
	LabelCapsLook->setFont(ISDefines::Gui::FONT_TAHOMA_8_BOLD);
	LayoutLabels->addWidget(LabelCapsLook);

	LayoutLabels->addStretch();

	LabelLang = new QLabel(this);
	LabelLang->setToolTip(LANG("CurrentLayout"));
	LabelLang->setCursor(CURSOR_WHATS_THIS);
	ISGui::SetFontWidgetBold(LabelLang, true);
	ISGui::SetFontWidgetUnderline(LabelLang, true);
	LayoutLabels->addWidget(LabelLang);

	Layout->addSpacerItem(new QSpacerItem(0, 55));

	LabelConnectToDatabase = new QLabel(this);
	Layout->addWidget(LabelConnectToDatabase, 0, Qt::AlignRight);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	Layout->addLayout(LayoutBottom);

	ButtonMenu = new ISServiceButton(this);
	ButtonMenu->setToolTip(LANG("Additionally"));
	ButtonMenu->setIcon(BUFFER_ICONS("Auth.Additionally"));
	ButtonMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	LayoutBottom->addWidget(ButtonMenu);

	ButtonMenu->menu()->addAction(BUFFER_ICONS("DatabaseConnection"), LANG("Form.Authorization.ConnectionSettings"), this, &ISAuthForm::ShowConnectionForm, Qt::Key_F9);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("About"), LANG("Form.Authorization.About"), this, &ISAuthForm::ShowAboutForm, Qt::Key_F1);

	LayoutBottom->addStretch();

	WaitWidget = new ISWaitWidget(this, false, false);
	WaitWidget->SetRoundness(70);
	WaitWidget->SetMinimumTrailOpacity(15);
	WaitWidget->SetTrailFadePercentage(70);
	WaitWidget->SetNumberOfLines(9);
	WaitWidget->SetLineLength(7);
	WaitWidget->SetLineWidth(2);
	WaitWidget->SetInnerRadius(4);
	WaitWidget->SetRevolutionsPerSecond(2);
	LayoutBottom->addWidget(WaitWidget);

	ButtonInput = new ISPushButton(BUFFER_ICONS("Apply.Blue"), LANG("Input"), this);
	ButtonInput->setToolTip(LANG("Input.ToolTip"));
	ButtonInput->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonInput, &ISPushButton::clicked, this, &ISAuthForm::Input);
	LayoutBottom->addWidget(ButtonInput);

	ButtonExit = new ISPushButton(BUFFER_ICONS("Auth.Exit"), LANG("Exit"), this);
	ButtonExit->setCursor(CURSOR_POINTING_HAND);
	ButtonExit->setToolTip(LANG("Exit.ToolTip"));
	//connect(ButtonExit, &ISPushButton::clicked, this, &ISAuthForm::close);
	connect(ButtonExit, &ISPushButton::clicked, &ISTcpConnector::Instance(), &ISTcpConnector::Disconnect);
	LayoutBottom->addWidget(ButtonExit);

	QAction *ActionClearFields = new QAction(this);
	ActionClearFields->setShortcut(Qt::Key_F12);
	connect(ActionClearFields, &QAction::triggered, this, &ISAuthForm::ClearFields);
	addAction(ActionClearFields);

	AuthConnector = new ISAuthConnector(this);
	connect(AuthConnector, &ISAuthConnector::ConnectedToHost, this, &ISAuthForm::ConnectedDone);
	connect(AuthConnector, &ISAuthConnector::ConnectedFailed, this, &ISAuthForm::ConnectedFailed);

#ifdef DEBUG
	EditLogin->SetValue("postgres");
	EditPassword->SetValue("adm777");
#else
	if (CONFIG_BOOL(CONST_CONFIG_REMEMBER_USER_INCLUDE))
	{
		EditLogin->SetValue(CONFIG_STRING(CONST_CONFIG_REMEMBER_USER_LOGIN));
	}
#endif
}
//-----------------------------------------------------------------------------
ISAuthForm::~ISAuthForm()
{
	
}
//-----------------------------------------------------------------------------
void ISAuthForm::closeEvent(QCloseEvent *CloseEvent)
{
	ConnectingState ? CloseEvent->ignore() : ISInterfaceDialogForm::closeEvent(CloseEvent);
}
//-----------------------------------------------------------------------------
void ISAuthForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	QTimer *TimerCapsLook = new QTimer(this);
	connect(TimerCapsLook, &QTimer::timeout, this, &ISAuthForm::TimeoutCapsLook);
	TimerCapsLook->start(200);

	QTimer *TimerLang = new QTimer(this);
	connect(TimerLang, &QTimer::timeout, this, &ISAuthForm::TimeoutLang);
	TimerLang->start(200);
}
//-----------------------------------------------------------------------------
void ISAuthForm::EnterClicked()
{
	Input();
}
//-----------------------------------------------------------------------------
void ISAuthForm::TimeoutCapsLook()
{
	if (ISGui::CheckPressCapsLook())
	{
		if (LabelCapsLook->text().isEmpty())
		{
			LabelCapsLook->setText(LANG("CapsLookActivate"));
		}
	}
	else
	{
		LabelCapsLook->setText(QString());
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::TimeoutLang()
{
	QString LayoutName = ISGui::GetCurrentLayoutName();
	if (LayoutName == "ENG")
	{
		LabelLang->setText("EN");
	}
	else if (LayoutName == "RUS")
	{
		LabelLang->setText("RU");
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::ShowConnectionForm()
{
	ISConnectionForm().Exec();
}
//-----------------------------------------------------------------------------
void ISAuthForm::ShowAboutForm()
{
	ISAboutForm().Exec();
}
//-----------------------------------------------------------------------------
void ISAuthForm::Input()
{
	if (Check())
	{
		SetConnecting(true);
		AuthConnector->Connect();
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::ConnectedDone()
{
	AuthConnector->Disconnect();
	SetConnecting(false);

	if (ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
		CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		EditLogin->GetValue().toString(), EditPassword->GetValue().toString())) //Если подключение к базе данных установлено
	{
		ISConfig::Instance().SetValue(CONST_CONFIG_REMEMBER_USER_INCLUDE, CheckRememberUser->GetValue());
		CheckRememberUser->GetValue().toBool() ?
			ISConfig::Instance().SetValue(CONST_CONFIG_REMEMBER_USER_LOGIN, EditLogin->GetValue()) :
			ISConfig::Instance().SetValue(CONST_CONFIG_REMEMBER_USER_LOGIN, QString());

		hide();
		SetResult(true);
		ISMetaUser::Instance().UserData->Login = EditLogin->GetValue().toString();
		ISMetaUser::Instance().UserData->Password = EditPassword->GetValue().toString();

		if (CONFIG_BOOL("Protocol/Use")) //Если используется протокол
		{
			QString Host = CONFIG_STRING("Protocol/Host");
			quint16 Port = CONFIG_INT("Protocol/Port");
			if (ISTcpConnector::Instance().Connect(Host, Port, EditLogin->GetValue().toString(), EditPassword->GetValue().toString()))
			{
				if (CONFIG_BOOL("Protocol/Auth"))
				{
					ISTcpQuery qAuth(API_AUTH);
					qAuth.BindValue("Login", EditLogin->GetValue().toString());
					qAuth.BindValue("Password", EditPassword->GetValue().toString());
					if (qAuth.Execute())
					{
						Port = qAuth.GetAnswer()["Port"].toInt();
					}
					else
					{
						ISMessageBox::ShowCritical(this, qAuth.GetErrorString());
					}

					if (!ISTcpConnector::Instance().Reconnect(Host, Port, EditLogin->GetValue().toString(), EditPassword->GetValue().toString()))
					{
						ISMessageBox::ShowCritical(this, ISTcpConnector::Instance().GetErrorString());
					}
				}
			}
			else
			{
				ISMessageBox::ShowCritical(this, ISTcpConnector::Instance().GetErrorString());
			}
			SetConnecting(false);
		}

		close();
	}
	else //Ошибка подключения к базе данных
	{
		ISMessageBox MessageBox(ISMessageBox::Warning,
			LANG("ErrorConnectionDatabase"),
			LANG("Message.Question.ConnectionError.Reconnect").arg(ISDatabase::Instance().GetErrorString()),
			ISMessageBox::Yes | ISMessageBox::No | ISMessageBox::Close, this);
		MessageBox.setButtonText(ISMessageBox::Close, LANG("Exit"));
		MessageBox.setDefaultButton(ISMessageBox::No);
		MessageBox.adjustSize();
		ISGui::MoveWidgetToDesktop(&MessageBox, ISNamespace::MWD_Center);
		ISMessageBox::StandardButtons ClickedButton = static_cast<ISMessageBox::StandardButtons>(MessageBox.Exec());
		if (ClickedButton == ISMessageBox::Yes) //Предложить повтор попытки соединения
		{
			Input();
		}
		else if (ClickedButton == ISMessageBox::No) //Отмена
		{
			return;
		}
		else if (ClickedButton == ISMessageBox::Close) //Выход из программы
		{
			SetResult(false);
			close();
			return;
		}
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::ConnectedFailed()
{
	SetConnecting(false);
	ISMessageBox::ShowWarning(this, LANG("Message.Warning.FailedConnectionToPostgreSQL").arg(AuthConnector->errorString().toLower()));
}
//-----------------------------------------------------------------------------
void ISAuthForm::SetConnecting(bool Connecting)
{
	ConnectingState = Connecting;
	ConnectingState ? WaitWidget->Start() : WaitWidget->Stop();
	ConnectingState ? LabelConnectToDatabase->setText(LANG("ConnectingToServer")) : LabelConnectToDatabase->clear();
	ISGui::SetWaitGlobalCursor(ConnectingState);
	EditLogin->setEnabled(!ConnectingState);
	EditPassword->setEnabled(!ConnectingState);
	CheckRememberUser->setEnabled(!ConnectingState);
	ButtonMenu->setEnabled(!ConnectingState);
	ButtonInput->setEnabled(!ConnectingState);
	ButtonExit->setEnabled(!ConnectingState);
}
//-----------------------------------------------------------------------------
bool ISAuthForm::Check()
{
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.ServerEmpty"));
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT).isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.PortEmpty"));
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.DatabaseNameEmpty"));
		return false;
	}

	if (EditLogin->GetValue().toString().isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Input.LoginEmpty"));
		EditLogin->BlinkRed();
		return false;
	}

	if (EditPassword->GetValue().toString().isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Input.PasswordEmpty"));
		EditPassword->BlinkRed();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISAuthForm::ClearFields()
{
	EditLogin->Clear();
	EditPassword->Clear();
	EditLogin->SetFocus();
}
//-----------------------------------------------------------------------------
