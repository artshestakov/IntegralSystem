#include "StdAfx.h"
#include "ISIntegralSystem.h"
#include "ISDefines.h"
#include "ISQueryExceptionSyntax.h"
#include "ISQueryExceptionConnection.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISReconnectForm.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISAutoLocking.h"
#include "ISSystem.h"
#include "ISTaskViewForm.h"
#include "ISObjectFormBase.h"
#include "ISGui.h"
#include "ISSplashWidget.h"
#include "ISLibraryLoader.h"
#include "ISConfig.h"
#include "ISCrashDumper.h"
#include "ISDebug.h"
#include "ISVersion.h"
//-----------------------------------------------------------------------------
ISIntegralSystem::ISIntegralSystem(int &argc, char **argv) : QApplication(argc,	argv)
{
	Q_INIT_RESOURCE(Resources);

	ISDebug::ShowString("Starting system... Version: " + ISVersion::GetInstance().GetVersion());

	//ISLibraryLoader LibraryLoader;
	//LibraryLoader.Load();

	ISCrashDumper::Startup();

	ISSplashWidget SplashWidget;
	SplashWidget.SetText("Starting system...");
	SplashWidget.show();
	ISSystem::ProcessEvents();

	//Загрузка конфигурационного файла
	ISConfig::GetInstance().Initialize(CONFIG_FILE_PATH);

	//Загрузка локализации
	SplashWidget.SetText("Loading localization...");
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_INTEGRAL_SYSTEM);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_OBJECTS);

	//Загрузка стилей интерфейса
	SplashWidget.SetText("Loading styles...");
	ISStyleSheet::GetInstance().Initialize();

	//Загрузка буфера
	SplashWidget.SetText("Loading buffer...");
	ISBuffer::GetInstance().Initialize();

	setEffectEnabled(Qt::UIEffect::UI_General);
	setEffectEnabled(Qt::UIEffect::UI_AnimateMenu);
	setEffectEnabled(Qt::UIEffect::UI_FadeMenu);
	setEffectEnabled(Qt::UIEffect::UI_AnimateCombo);
	setEffectEnabled(Qt::UIEffect::UI_AnimateTooltip);
	setEffectEnabled(Qt::UIEffect::UI_FadeTooltip);
	setEffectEnabled(Qt::UIEffect::UI_AnimateToolBox);

	setStyleSheet(STYLE_SHEET("QToolTip"));
	setWindowIcon(BUFFER_ICONS("Logo"));
	setApplicationName("IntegralSystem");
	setApplicationVersion(ISVersion::GetInstance().GetVersion());
	setFont(FONT_APPLICATION);

	QToolTip::setFont(FONT_APPLICATION);

	SplashWidget.close();
}
//-----------------------------------------------------------------------------
ISIntegralSystem::~ISIntegralSystem()
{
	
}
//-----------------------------------------------------------------------------
bool ISIntegralSystem::CheckAdminRole()
{
	ISDebug::ShowDebugString("Check administrator role...");

	QFile File(APPLICATION_TEMP_PATH + "/CheckAdmin");
	bool Opened = File.open(QIODevice::WriteOnly);

	if (Opened)
	{
		File.close();
		File.remove();
	}
	else
	{
		if (File.error() != QFileDevice::OpenError)
		{
			Opened = true;
		}
	}

	if (Opened)
	{
		ISDebug::ShowDebugString("Administrator role exist");
	}
	else
	{
		ISDebug::ShowDebugString("Administrator role not exist");
	}

	return Opened;
}
//-----------------------------------------------------------------------------
bool ISIntegralSystem::notify(QObject *Object, QEvent *e)
{
	bool Result = false;

	try
	{
		Result = QApplication::notify(Object, e);
		
		if (ISAutoLocking::GetInstance().GetInitialized()) //Если класс автоблокировки инициализирован
		{
			if (!ISAutoLocking::GetInstance().GetLock()) //Если программа не заблокирована - реагировать на события
			{
				switch (e->type())
				{
				case QEvent::Move: ISAutoLocking::GetInstance().RestartTimer(); break;
				case QEvent::MouseMove: ISAutoLocking::GetInstance().RestartTimer(); break;
				case QEvent::Show: ISAutoLocking::GetInstance().RestartTimer(); break;
				case QEvent::Hide: ISAutoLocking::GetInstance().RestartTimer(); break;
				case QEvent::WindowActivate: ISAutoLocking::GetInstance().RestartTimer(); break;
				case QEvent::WindowDeactivate: ISAutoLocking::GetInstance().RestartTimer(); break;
				}
			}
		}
	}
	catch (ISQueryExceptionSyntax &QueryExceptionSyntax)
	{
		ISSystem::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(nullptr, LOCALIZATION("Message.Warning.QueryExceptionSyntax"), QueryExceptionSyntax.GetWhat());
	}
	catch (ISQueryExceptionConnection &QueryExceptionConnection)
	{
		ISSystem::SetWaitGlobalCursor(false);

		if (ISMessageBox::ShowQuestion(nullptr, LOCALIZATION("Message.Question.Reconnect")))
		{
			ISReconnectForm::GetInstance().show();
			ISReconnectForm::GetInstance().StartReconnect(Result);
			if (Result)
			{
				ISMessageBox::ShowInformation(nullptr, LOCALIZATION("Message.Information.Reconnect.Done"));
			}
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISIntegralSystem::ShowTaskViewForm(int TaskID)
{
	ISSystem::SetWaitGlobalCursor(true);
	ISTaskViewForm *TaskViewForm = new ISTaskViewForm(TaskID);
	TaskViewForm->showMaximized();
	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISIntegralSystem::ShowTaskObjectForm(ISNamespace::ObjectFormType FormType, int TaskID)
{
	ISObjectFormBase *TaskObjectForm = ISGui::CreateObjectForm(FormType, "_Task", TaskID);
	ShowTaskObjectForm(TaskObjectForm);
}
//-----------------------------------------------------------------------------
void ISIntegralSystem::ShowTaskObjectForm(QWidget *TaskObjectForm)
{
	TaskObjectForm->setParent(nullptr);
	TaskObjectForm->resize(SIZE_TASK_OBJECT_FORM);
	ISSystem::MoveWidgetToDesktop(TaskObjectForm, ISNamespace::MWD_Center);
	TaskObjectForm->show();
}
//-----------------------------------------------------------------------------
