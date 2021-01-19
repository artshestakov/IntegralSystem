#include "ISAuthDialog.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISAboutDialog.h"
#include "ISConfig.h"
#include "ISConnectDialog.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISVersionInfo.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISObjects.h"
#include "ISProcessForm.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISAuthDialog::ISAuthDialog()
	: ISInterfaceDialogForm(true),
	ConnectingState(false)
{
	setWindowTitle(LANG("InputInSystem"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QLabel *LableImage = new QLabel(this);
	LableImage->setScaledContents(true);
	LableImage->setPixmap(BUFFER_PIXMAPS("BannerLogoAuth"));
	GetMainLayout()->addWidget(LableImage);

	QVBoxLayout *LayoutFields = new QVBoxLayout();
	LayoutFields->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(LayoutFields);

	LayoutFields->addWidget(new QLabel(LANG("InputLoginAndPassword") + ':', this));

	EditLogin = new ISLineEdit(this);
	EditLogin->SetPlaceholderText(LANG("Login"));
	EditLogin->SetIcon(BUFFER_ICONS("Auth.Login"));
	EditLogin->SetRegExp(REG_EXP_LOGIN);
	LayoutFields->addWidget(EditLogin);

	EditPassword = new ISLineEdit(this);
	EditPassword->SetPlaceholderText(LANG("Password"));
	EditPassword->SetIcon(BUFFER_ICONS("Auth.Password"));
	EditPassword->SetEchoMode(QLineEdit::Password);
	LayoutFields->addWidget(EditPassword);

	CheckRememberUser = new ISCheckEdit(this);
	CheckRememberUser->SetText(LANG("RememberMe"));
	CheckRememberUser->SetToolTip(LANG("RememberMe.ToolTip"));
	CheckRememberUser->SetValue(CONFIG_BOOL(CONST_CONFIG_REMEMBER_USER_INCLUDE));
	LayoutFields->addWidget(CheckRememberUser);

	LayoutFields->addSpacerItem(new QSpacerItem(0, 55));

	LabelIndicator = new QLabel(this);
	LayoutFields->addWidget(LabelIndicator, 0, Qt::AlignRight);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutFields->addLayout(LayoutBottom);

	ButtonMenu = new ISServiceButton(BUFFER_ICONS("Auth.Additionally"), LANG("Additionally"), this);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	LayoutBottom->addWidget(ButtonMenu);

	ButtonMenu->menu()->addAction(BUFFER_ICONS("DatabaseConnection"), LANG("Form.Authorization.ConnectionSettings"), this, &ISAuthDialog::ShowConnectionForm, Qt::Key_F9);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("About"), LANG("Form.Authorization.About"), this, &ISAuthDialog::ShowAboutForm, Qt::Key_F1);

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
	connect(ButtonInput, &ISPushButton::clicked, this, &ISAuthDialog::Input);
	LayoutBottom->addWidget(ButtonInput);

	ButtonExit = new ISPushButton(BUFFER_ICONS("Auth.Exit"), LANG("Exit"), this);
	ButtonExit->setCursor(CURSOR_POINTING_HAND);
	ButtonExit->setToolTip(LANG("Exit.ToolTip"));
	connect(ButtonExit, &ISPushButton::clicked, this, &ISAuthDialog::close);
	LayoutBottom->addWidget(ButtonExit);

#ifdef DEBUG
	EditLogin->SetValue(SYSTEM_USER_LOGIN);
	EditPassword->SetValue("adm777");
#else
	if (CONFIG_BOOL(CONST_CONFIG_REMEMBER_USER_INCLUDE))
	{
		EditLogin->SetValue(CONFIG_STRING(CONST_CONFIG_REMEMBER_USER_LOGIN));
		QTimer::singleShot(100, EditPassword, &ISLineEdit::SetFocus);
	}
#endif
}
//-----------------------------------------------------------------------------
ISAuthDialog::~ISAuthDialog()
{

}
//-----------------------------------------------------------------------------
void ISAuthDialog::closeEvent(QCloseEvent *CloseEvent)
{
	ConnectingState ? CloseEvent->ignore() : ISInterfaceDialogForm::closeEvent(CloseEvent);
}
//-----------------------------------------------------------------------------
void ISAuthDialog::EnterClicked()
{
	Input();
}
//-----------------------------------------------------------------------------
void ISAuthDialog::ShowConnectionForm()
{
	ISConnectDialog().Exec();
}
//-----------------------------------------------------------------------------
void ISAuthDialog::ShowAboutForm()
{
	ISAboutDialog().Exec();
}
//-----------------------------------------------------------------------------
void ISAuthDialog::Input()
{
	//Проверяем заполнение конфигурационного файла
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.ServerEmpty"));
		return;
	}
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT).isEmpty())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectionSetting.Input.PortEmpty"));
		return;
	}
	if (EditLogin->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Input.LoginEmpty"));
		EditLogin->BlinkRed();
		return;
	}
	if (EditPassword->GetValue().toString().isEmpty())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Input.PasswordEmpty"));
		EditPassword->BlinkRed();
		return;
	}

	SetConnecting(true);
	if (!ISTcpConnector::Instance().Connect(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT))) //Ошибка подключения к карату
	{
		SetConnecting(false);
		ISMessageBox MessageBox(ISMessageBox::Question, LANG("Question"),
			LANG("Message.Question.ConnectionError.Reconnect").arg(ISTcpConnector::Instance().GetErrorString()), QString(),
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
		return;
	}

	ISTcpQuery qAuth(API_AUTH);
	qAuth.BindValue("Hash", ISSystem::StringToSha256(EditLogin->GetValue().toString() + EditPassword->GetValue().toString()));
	qAuth.BindValue("Version", ISVersionInfo::Instance().Info.Version);
	if (qAuth.Execute()) //Авторизация прошла успешно
	{
		QVariantMap AnswerMap = qAuth.GetAnswer();
		QVariantMap UpdateClientMap = AnswerMap["UpdateClient"].toMap();

		if (UpdateClientMap["IsNeed"].toBool()) //Если требуется обновление - предлагаем скачать и установить
		{
			SetConnecting(false);
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.UpdateAvailable"), LANG("Message.Question.UpdateAvailable.DetailedText")
				.arg(ISVersionInfo::Instance().Info.Version).arg(UpdateClientMap["NewVersion"].toUInt()))) //Пользователь согласился
			{
				ISProcessForm ProcessForm(LANG("UploadingUpdate"), this);
				ProcessForm.show();
				if (qAuth.Execute(API_GET_LAST_CLIENT)) //Запрос на обновление прошёл успешно
				{
					ProcessForm.SetText(LANG("SavingUpdate"));
					AnswerMap = qAuth.GetAnswer();
					QFile File(QCoreApplication::applicationDirPath() + "/Temp/" + AnswerMap["FileName"].toString());
					if (File.open(QIODevice::WriteOnly))
					{
						File.write(QByteArray::fromBase64(AnswerMap["Data"].toByteArray()));
						File.close();
						ProcessForm.hide();
						ISMessageBox::ShowInformation(this, LANG("Message.Information.InstallUpdate"));
						if (!QProcess::startDetached(File.fileName(), QStringList() << "/SILENT" << "/NOCANCEL" << "/NORESTART")) //Не удалось запустить установку
						{
							ISMessageBox::ShowWarning(this, LANG("Message.Warning.StartInstallUpdate").arg(File.fileName()));
						}
						close();
						return;
					}
					else //Не удалось сохранить обновление
					{
						SetConnecting(false);
						ISMessageBox::ShowCritical(&ProcessForm, LANG("Message.Error.SaveUpdate"), File.errorString());
					}
				}
				else //Ошибка запроса на обновление
				{
					SetConnecting(false);
					ISMessageBox::ShowCritical(&ProcessForm, LANG("Message.Error.DownloadUpdate"), qAuth.GetErrorString());
				}
			}
		}

		//Получаем информацию о текущем пользователе
		ISBuffer::Instance().CurrentUserInfo.System = AnswerMap["UserIsSystem"].toBool();
		ISBuffer::Instance().CurrentUserInfo.ID = AnswerMap["UserID"].toUInt();
		ISBuffer::Instance().CurrentUserInfo.FIO = AnswerMap["UserFIO"].toString();
		ISBuffer::Instance().CurrentUserInfo.Login = EditLogin->GetValue().toString();
		ISBuffer::Instance().CurrentUserInfo.Password = EditPassword->GetValue().toString();
		ISBuffer::Instance().CurrentUserInfo.GroupID = AnswerMap["UserGroupID"].toUInt();
		ISBuffer::Instance().CurrentUserInfo.GroupFullAccess = AnswerMap["UserGroupFullAccess"].toBool();
		
		//Получаем информацию о конфигурации
		QVariantMap ConfigurationInfo = AnswerMap["Configuration"].toMap();
		ISBuffer::Instance().ConfigurationInfo.Name = ConfigurationInfo["Name"].toString();
		ISBuffer::Instance().ConfigurationInfo.UID = ConfigurationInfo["UID"];
		ISBuffer::Instance().ConfigurationInfo.LocalName = ConfigurationInfo["LocalName"].toString();
		ISBuffer::Instance().ConfigurationInfo.DesktopForm = ConfigurationInfo["DesktopForm"].toString();
		ISBuffer::Instance().ConfigurationInfo.DateExpired = QDate::fromString(ConfigurationInfo["DateExpired"].toString(), FORMAT_DATE_V2);
		ISBuffer::Instance().ConfigurationInfo.LogoName = ConfigurationInfo["LogoName"].toString();

		SetConnecting(false);
		SetResult(true);
		hide();
		close();
	}
	else //Ошибка авторизации
	{
		SetConnecting(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.Auth"), qAuth.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISAuthDialog::SetConnecting(bool Connecting)
{
	ConnectingState = Connecting;
	ConnectingState ? WaitWidget->Start() : WaitWidget->Stop();
	ConnectingState ? LabelIndicator->setText(LANG("ConnectingToServer")) : LabelIndicator->clear();
	ISGui::SetWaitGlobalCursor(ConnectingState);
	EditLogin->setEnabled(!ConnectingState);
	EditPassword->setEnabled(!ConnectingState);
	CheckRememberUser->setEnabled(!ConnectingState);
	ButtonMenu->setEnabled(!ConnectingState);
	ButtonInput->setEnabled(!ConnectingState);
	ButtonExit->setEnabled(!ConnectingState);
}
//-----------------------------------------------------------------------------
