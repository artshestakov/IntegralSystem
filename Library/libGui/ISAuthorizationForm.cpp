#include "StdAfx.h"
#include "ISAuthorizationForm.h"
#include "ISCore.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISAboutForm.h"
#include "ISSplashScreen.h"
#include "ISMetaUser.h"
#include "ISConfig.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISConnectionForm.h"
#include "ISMessageBox.h"
#include "ISControls.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISAuthorizationForm::ISAuthorizationForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	AuthConnector = nullptr;

	setWindowTitle(LOCALIZATION("InputInSystem"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_NULL);

	QLabel *LableImage = new QLabel(this);
	LableImage->setScaledContents(true);
	LableImage->setPixmap(BUFFER_PIXMAPS("BannerLogoAuth"));
	GetMainLayout()->addWidget(LableImage);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_10_PX);
	GetMainLayout()->addLayout(Layout);

	Layout->addWidget(new QLabel(LOCALIZATION("InputLoginAndPassword") + ":", this));

	EditLogin = new ISLineEdit(this);
	EditLogin->SetPlaceholderText(LOCALIZATION("Login"));
	EditLogin->SetIcon(BUFFER_ICONS("Auth.Login"));
	EditLogin->SetVisibleClear(false);
	EditLogin->SetRegExp(REG_EXP_LOGIN);
	Layout->addWidget(EditLogin);

	EditPassword = new ISPasswordEdit(this);
	EditPassword->SetPlaceholderText(LOCALIZATION("Password"));
	EditPassword->SetVisibleGenerate(false);
	EditPassword->SetIcon(BUFFER_ICONS("Auth.Password"));
	EditPassword->SetVisibleCheckBox(false);
	EditPassword->SetVisibleClear(false);
	Layout->addWidget(EditPassword);

	QHBoxLayout *LayoutLabels = new QHBoxLayout();
	LayoutLabels->setContentsMargins(LAYOUT_MARGINS_NULL);
	Layout->addLayout(LayoutLabels);

	LabelCapsLook = new QLabel(this);
	LabelCapsLook->setFont(FONT_TAHOMA_8);
	LayoutLabels->addWidget(LabelCapsLook);

	LayoutLabels->addStretch();

	LabelLang = new QLabel(this);
	LabelLang->setToolTip(LOCALIZATION("CurrentLayout"));
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
	ButtonMenu->setToolTip(LOCALIZATION("Additionally"));
	ButtonMenu->setIcon(BUFFER_ICONS("Auth.Additionally"));
	ButtonMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	LayoutBottom->addWidget(ButtonMenu);

	ButtonMenu->menu()->addAction(LOCALIZATION("Form.Authorization.ConnectionSettings"), this, &ISAuthorizationForm::ShowConnectionForm, Qt::Key_F9);
	ButtonMenu->menu()->addAction(LOCALIZATION("Form.Authorization.About"), this, &ISAuthorizationForm::ShowAboutForm, Qt::Key_F1);

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

	ButtonInput = new ISPushButton(BUFFER_ICONS("Apply.Blue"), LOCALIZATION("Input"), this);
	ButtonInput->setToolTip(LOCALIZATION("Input.ToolTip"));
	ButtonInput->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonInput, &ISPushButton::clicked, this, &ISAuthorizationForm::Input);
	LayoutBottom->addWidget(ButtonInput);

	ButtonExit = new ISPushButton(BUFFER_ICONS("Auth.Exit"), LOCALIZATION("Exit"), this);
	ButtonExit->setCursor(CURSOR_POINTING_HAND);
	ButtonExit->setToolTip(LOCALIZATION("Exit.ToolTip"));
	connect(ButtonExit, &ISPushButton::clicked, this, &ISAuthorizationForm::close);
	LayoutBottom->addWidget(ButtonExit);

	QAction *ActionClearFields = new QAction(this);
	ActionClearFields->setShortcut(Qt::Key_F12);
	connect(ActionClearFields, &QAction::triggered, this, &ISAuthorizationForm::ClearFields);
	addAction(ActionClearFields);
}
//-----------------------------------------------------------------------------
ISAuthorizationForm::~ISAuthorizationForm()
{
	
}
//-----------------------------------------------------------------------------
QString ISAuthorizationForm::GetEnteredLogin() const
{
	return EditLogin->GetValue().toString();
}
//-----------------------------------------------------------------------------
QString ISAuthorizationForm::GetEnteredPassword() const
{
	return EditPassword->GetValue().toString();
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();

	QTimer *TimerCapsLook = new QTimer(this);
	connect(TimerCapsLook, &QTimer::timeout, this, &ISAuthorizationForm::TimeoutCapsLook);
	TimerCapsLook->start(100);

	QTimer *TimerLang = new QTimer(this);
	connect(TimerLang, &QTimer::timeout, this, &ISAuthorizationForm::TimeoutLang);
	TimerLang->start(100);

	if (CONFIG_BOOL(CONST_CONFIG_AUTOINPUT_INCLUDED))
	{
		EditLogin->SetValue(CONFIG_VALUE(CONST_CONFIG_AUTOINPUT_LOGIN));
		EditPassword->SetValue(CONFIG_VALUE(CONST_CONFIG_AUTOINPUT_PASSWORD));

		if (EditPassword->GetValue().toString().length())
		{
			ButtonInput->setFocus();
		}
		else
		{
			EditPassword->SetFocus();
		}
	}
	else
	{
		EditLogin->SetFocus();
	}
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::EnterClicked()
{
	Input();
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::TimeoutCapsLook()
{
	if (ISGui::CheckPressCapsLook())
	{
		if (!LabelCapsLook->text().length())
		{
			LabelCapsLook->setText(LOCALIZATION("CapsLookActivate"));
		}
	}
	else
	{
		LabelCapsLook->setText(QString());
	}
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::TimeoutLang()
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
void ISAuthorizationForm::ShowConnectionForm()
{
	ISConnectionForm ConnectionForm;
	connect(&ConnectionForm, &ISConnectionForm::AutiInputChanged, [=](const QString &Login, const QString &Password)
	{
		EditLogin->SetValue(Login);
		EditPassword->SetValue(Password);
	});
	ConnectionForm.Exec();
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::ShowAboutForm()
{
	ISAboutForm AboutForm;
	AboutForm.Exec();
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::Input()
{
	if (Check())
	{
		SetConnecting(true);

		if (!AuthConnector)
		{
			AuthConnector = new ISAuthConnector(this);
			connect(AuthConnector, &ISAuthConnector::ConnectedToHost, this, &ISAuthorizationForm::ConnectedDone);
			connect(AuthConnector, &ISAuthConnector::ConnectedFailed, this, &ISAuthorizationForm::ConnectedFailed);
		}

		AuthConnector->Connect();
	}
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::ConnectedDone()
{
	AuthConnector->Disconnect();
	SetConnecting(false);

	QString ErrorConnection;
	if (ISDatabase::GetInstance().ConnectToDefaultDB(EditLogin->GetValue().toString(), EditPassword->GetValue().toString(), ErrorConnection)) //Подключение к базе данных установлено
	{
		hide();
		SetResult(true);
		close();
	}
	else //Ошибка подключения к базе данных
	{
		ISMessageBox MessageBox(ISMessageBox::Warning, LOCALIZATION("ErrorConnectionDatabase"), LOCALIZATION("Message.Question.ConnectionError.Reconnect").arg(ErrorConnection), ISMessageBox::Yes | ISMessageBox::No | ISMessageBox::Close, this);
		MessageBox.setButtonText(ISMessageBox::Close, LOCALIZATION("Exit"));
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
void ISAuthorizationForm::ConnectedFailed()
{
	SetConnecting(false);
	ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.SslConnectionDBFailed").arg(AuthConnector->errorString().toLower()));
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::SetConnecting(bool Connecting)
{
	if (Connecting)
	{
		WaitWidget->Start();
		LabelConnectToDatabase->setText(LOCALIZATION("ConnectingToServer") + "...");
	}
	else
	{
		WaitWidget->Stop();
		LabelConnectToDatabase->setText(QString());
	}

	ISGui::SetWaitGlobalCursor(Connecting);
	EditLogin->setEnabled(!Connecting);
	EditPassword->setEnabled(!Connecting);
	ButtonMenu->setEnabled(!Connecting);
	ButtonInput->setEnabled(!Connecting);
	ButtonExit->setEnabled(!Connecting);
}
//-----------------------------------------------------------------------------
bool ISAuthorizationForm::Check()
{
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Error.ConnectionSetting.Input.ServerEmpty"));
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT).isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Error.ConnectionSetting.Input.PortEmpty"));
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LOCALIZATION("Message.Error.ConnectionSetting.Input.DatabaseNameEmpty"));
		return false;
	}

	if (!EditLogin->GetValue().toString().length())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Input.LoginEmpty"));
		EditLogin->BlinkRed();
		return false;
	}

	if (!EditPassword->GetValue().toString().length())
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Error.Input.PasswordEmpty"));
		EditPassword->BlinkRed();
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ISAuthorizationForm::ClearFields()
{
	EditLogin->Clear();
	EditPassword->Clear();
	EditLogin->SetFocus();
}
//-----------------------------------------------------------------------------
