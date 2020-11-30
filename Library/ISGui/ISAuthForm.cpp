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
#include "ISVersionInfo.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISObjects.h"
#include "ISProcessForm.h"
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

	QVBoxLayout *LayoutFields = new QVBoxLayout();
	LayoutFields->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(LayoutFields);

	LayoutFields->addWidget(new QLabel(LANG("InputLoginAndPassword") + ':', this));

	EditLogin = new ISLineEdit(this);
	EditLogin->SetPlaceholderText(LANG("Login"));
	EditLogin->SetIcon(BUFFER_ICONS("Auth.Login"));
	EditLogin->SetRegExp(REG_EXP_LOGIN);
	LayoutFields->addWidget(EditLogin);

	EditPassword = new ISPasswordEdit(this);
	EditPassword->SetPlaceholderText(LANG("Password"));
	EditPassword->SetVisibleGenerate(false);
	EditPassword->SetIcon(BUFFER_ICONS("Auth.Password"));
	EditPassword->SetVisibleCheckBox(false);
	LayoutFields->addWidget(EditPassword);

	CheckRememberUser = new ISCheckEdit(this);
	CheckRememberUser->SetText(LANG("RememberMe"));
	CheckRememberUser->SetToolTip(LANG("RememberMe.ToolTip"));
	CheckRememberUser->SetValue(CONFIG_BOOL(CONST_CONFIG_REMEMBER_USER_INCLUDE));
	LayoutFields->addWidget(CheckRememberUser);

	LayoutFields->addSpacerItem(new QSpacerItem(0, 55));

	QHBoxLayout *LayoutIndicator = new QHBoxLayout();
	LayoutIndicator->addStretch();
	LayoutFields->addLayout(LayoutIndicator);

	WaitWidget = new ISWaitWidget(this, false, false);
	WaitWidget->SetRoundness(70);
	WaitWidget->SetMinimumTrailOpacity(15);
	WaitWidget->SetTrailFadePercentage(70);
	WaitWidget->SetNumberOfLines(9);
	WaitWidget->SetLineLength(7);
	WaitWidget->SetLineWidth(2);
	WaitWidget->SetInnerRadius(4);
	WaitWidget->SetRevolutionsPerSecond(2);
	LayoutIndicator->addWidget(WaitWidget);

	LabelIndicator = new QLabel(this);
	LayoutIndicator->addWidget(LabelIndicator);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutFields->addLayout(LayoutBottom);

	ButtonMenu = new ISServiceButton(BUFFER_ICONS("Auth.Additionally"), LANG("Additionally"), this);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	LayoutBottom->addWidget(ButtonMenu);

	ButtonMenu->menu()->addAction(BUFFER_ICONS("DatabaseConnection"), LANG("Form.Authorization.ConnectionSettings"), this, &ISAuthForm::ShowConnectionForm, Qt::Key_F9);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("About"), LANG("Form.Authorization.About"), this, &ISAuthForm::ShowAboutForm, Qt::Key_F1);

	LayoutBottom->addStretch();

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
		ISInterfaceDialogForm::closeEvent(CloseEvent);
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::EnterClicked()
{
	Input();
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
	//Проверяем наличие обновлений
	if (CheckUpdate())
	{
		close();
		return;
	}

	if (Check())
	{
		//Если выбран вход по протоколу - используем новую функцию, иначе - старую (через БД)
		CONFIG_BOOL("Protocol/Include") ? InputNew() : InputOld();
	}
}
//-----------------------------------------------------------------------------
void ISAuthForm::InputOld()
{
	SetConnecting(true);
	AuthConnector->Connect();
}
//-----------------------------------------------------------------------------
void ISAuthForm::InputNew()
{
	if (!ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).isOpen()) //Если подключения ещё нет - подключаемся
	{
		if (!ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
			CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
			EditLogin->GetValue().toString(), EditPassword->GetValue().toString())) //Если подключение к базе данных установлено
		{
			ISMessageBox::ShowCritical(this, ISDatabase::Instance().GetErrorString());
			return;
		}
	}

	if (!ISTcpConnector::Instance().Connect(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT("Protocol/Port"))) //Ошибка подключения к карату
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ConnectToServer"), ISTcpConnector::Instance().GetErrorString());
		return;
	}

	ISTcpQuery qAuth(API_AUTH);
	qAuth.BindValue("Hash", ISSystem::StringToSha256(EditLogin->GetValue().toString() + EditPassword->GetValue().toString()));
	qAuth.BindValue("Version", ISVersionInfo::Instance().ToStringVersion());
	if (qAuth.Execute()) //Авторизация прошла успешно
	{
		QVariantMap AnswerMap = qAuth.GetAnswer();
		if (AnswerMap["IsNeedUpdate"].toBool()) //Если требуется обновление - предлагаем скачать и установить
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.UpdateAvailable"))) //Пользователь согласился
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
						ISMessageBox::ShowCritical(&ProcessForm, LANG("Message.Error.SaveUpdate"), File.errorString());
					}
				}
				else //Ошибка запроса на обновление
				{
					ISMessageBox::ShowCritical(&ProcessForm, LANG("Message.Error.DownloadUpdate"), qAuth.GetErrorString());
				}
			}
		}
		ISMetaUser::Instance().UserData.System = AnswerMap["UserIsSystem"].toBool();
		ISMetaUser::Instance().UserData.ID = AnswerMap["UserID"].toUInt();
		ISMetaUser::Instance().UserData.FIO = AnswerMap["UserFIO"].toString();
		ISMetaUser::Instance().UserData.Login = EditLogin->GetValue().toString();
		ISMetaUser::Instance().UserData.Password = EditPassword->GetValue().toString();
		ISMetaUser::Instance().UserData.GroupID = AnswerMap["UserGroupID"].toUInt();
		ISMetaUser::Instance().UserData.GroupFullAccess = AnswerMap["UserGroupFullAccess"].toBool();
		ISVersionInfo::Instance().ConfigurationInfo.UID = AnswerMap["Configuration"].toMap()["UID"];
		ISVersionInfo::Instance().ConfigurationInfo.Name = AnswerMap["Configuration"].toMap()["Name"].toString();
		ISVersionInfo::Instance().ConfigurationInfo.LocalName = AnswerMap["Configuration"].toMap()["Local"].toString();
		ISVersionInfo::Instance().ConfigurationInfo.DesktopForm = AnswerMap["Configuration"].toMap()["Desktop"].toString();
		ISVersionInfo::Instance().ConfigurationInfo.LogoName = AnswerMap["Configuration"].toMap()["Logo"].toString();
		SetResult(true);
		hide();
		close();
	}
	else //Ошибка авторизации
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.Auth"), qAuth.GetErrorString());
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
		ISMetaUser::Instance().UserData.Login = EditLogin->GetValue().toString();
		ISMetaUser::Instance().UserData.Password = EditPassword->GetValue().toString();
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
	if (result) //Если путь к папке не пустой - проверяем обновления
	{
		//Получаем список файлов в папке и обходим каждый из них
		QStringList StringList = QDir(UpdateDir).entryList(QDir::Files, QDir::Time);
		result = !StringList.isEmpty();
		if (result)
		{
			QFileInfo FileInfo(StringList.front());
			StringList = FileInfo.fileName().split(SYMBOL_POINT);
			result = StringList.size() == 4;
			if (result)
			{
				result = StringList[2].toUInt() > ISVersionInfo::Instance().Info.Version.GetRevision();
				if (result)
				{
					ISLOGGER_I(__CLASS__, QString("Founded update. This version: %1. Update file: %2").arg(ISVersionInfo::Instance().ToStringVersion()).arg(FileInfo.fileName()));
					ISMessageBox::ShowInformation(this, LANG("Message.Information.FoundNewAppVersion"));
					QString FilePath = UpdateDir + '/' + FileInfo.fileName();
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
