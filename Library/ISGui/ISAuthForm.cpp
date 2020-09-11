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
#include "ISVersion.h"
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
	CheckRememberUser->SetToolTip(LANG("RememberMe.ToolTip"));
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

	ButtonMenu = new ISServiceButton(BUFFER_ICONS("Auth.Additionally"), LANG("Additionally"), this);
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
	connect(ButtonExit, &ISPushButton::clicked, this, &ISAuthForm::close);
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
		QTimer::singleShot(100, EditPassword, &ISPasswordEdit::SetFocus);
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
	if (ConnectingState)
	{
		CloseEvent->ignore();
	}
	else
	{
		TimerCapsLook->stop();
		TimerLang->stop();
		ISInterfaceDialogForm::closeEvent(CloseEvent);
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	TimerCapsLook = new QTimer(this);
	connect(TimerCapsLook, &QTimer::timeout, this, &ISAuthForm::TimeoutCapsLook);
	TimerCapsLook->start(200);

	TimerLang = new QTimer(this);
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
	if (CheckUpdate())
	{
		close();
		return;
	}

	if (Check())
	{
		SetConnecting(true);
		if (CONFIG_BOOL("Protocol/Use")) //Используем протокол
		{
			QString Host = CONFIG_STRING("Protocol/Host");
			quint16 Port = CONFIG_INT("Protocol/Port");
			if (!ISTcpConnector::Instance().Connect(Host, Port)) //Ошибка подключения к карату
			{
				SetConnecting(false);
				ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectToServer"), ISTcpConnector::Instance().GetErrorString());
				return;
			}

			if (CONFIG_BOOL("Protocol/Auth")) //Используем авторизацию
			{
				ISTcpQuery qAuth(API_AUTH);
				qAuth.BindValue("Login", EditLogin->GetValue().toString());
				qAuth.BindValue("Password", EditPassword->GetValue().toString());
				if (qAuth.Execute()) //Запрос на авторизацию прошёл успешно
				{
					Port = qAuth.GetAnswer()["Port"].toInt();
					if (!ISTcpConnector::Instance().Reconnect(Host, Port)) //Подключение в воркеру прошло успешно
					{
						SetConnecting(false);
						ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectToWorker"), ISTcpConnector::Instance().GetErrorString());
					}
				}
				else //Ошибка авторизации
				{
					SetConnecting(false);
					ISMessageBox::ShowCritical(this, LANG("Message.Error.Auth").arg(LANG(qAuth.GetErrorString())));
				}
			}
			ConnectedDone();
		}
		else //Стандартное подключение
		{
			AuthConnector->Connect();
		}
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
		close();
	}
	else //Ошибка подключения к базе данных
	{
		ISMessageBox MessageBox(ISMessageBox::Question, LANG("Question"),
			LANG("Message.Question.ConnectionError.Reconnect").arg(ISDatabase::Instance().GetErrorString()), QString(),
		{
			{ 1, LANG("Yes") },
			{ 2, LANG("No"), true }, //Нажатие на эту кнопку не учитывается
			{ 3, LANG("Exit") }
		}, this);
		switch (MessageBox.Exec())
		{
		case 1: Input(); break; //Выбрали повторить подключение
		case 3: close(); break; //Выбрали выход из программы
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
bool ISAuthForm::CheckUpdate()
{
	QString UpdateDir = CONFIG_STRING(CONST_CONFIG_CONNECTION_UPDATE_DIR);
	bool result = !UpdateDir.isEmpty();
	if (result)
	{
		QFileInfoList FileInfoList = QDir(UpdateDir).entryInfoList(QStringList() << "*.exe", QDir::Files, QDir::Name);
		for (const QFileInfo &FileInfo : FileInfoList)
		{
			QString FileName = FileInfo.fileName();
			QStringList StringList = FileName.split(SYMBOL_POINT);
			result = StringList.size() == 4;
			if (result)
			{
				result = StringList[2].toInt() > ISVersion::Instance().Info.Revision;
				if (result)
				{
					ISMessageBox::ShowInformation(this, LANG("Message.Information.FoundNewAppVersion"));
					QString FilePath = FileInfo.filePath();
					result = QProcess::startDetached(FilePath, QStringList() << "/SILENT" << "/NOCANCEL" << "/NORESTART");
					if (!result)
					{
						ISMessageBox::ShowWarning(this, LANG("Message.Warning.StartInstallUpdate").arg(FilePath));
					}
				}
			}
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
