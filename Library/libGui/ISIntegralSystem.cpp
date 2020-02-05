#include "ISIntegralSystem.h"
#include "EXDefines.h"
#include "ISQueryExceptionSyntax.h"
#include "ISQueryExceptionConnection.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISReconnectForm.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISAutoLocking.h"
#include "ISGui.h"
#include "ISSplashWidget.h"
#include "ISConfig.h"
#include "EXCrashDumper.h"
#include "ISDebug.h"
#include "ISVersion.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
ISIntegralSystem::ISIntegralSystem(int &argc, char **argv) : QApplication(argc,	argv)
{
	QString ErrorString;
	ISCore::Startup(true, ErrorString);

	ISDebug::ShowString("Starting system... Version: " + ISVersion::GetInstance().GetVersion());

	ISSplashWidget SplashWidget;
	SplashWidget.SetText("Starting system...");
	SplashWidget.show();
	ISGui::ProcessEvents();

	//Загрузка конфигурационного файла
	ISConfig::GetInstance().Initialize();

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

	QFile File(PATH_TEMP_DIR + "/CheckAdmin");
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
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.QueryExceptionSyntax"), QueryExceptionSyntax.GetWhat());
	}
	catch (ISQueryExceptionConnection &QueryExceptionConnection)
	{
		ISGui::SetWaitGlobalCursor(false);

		if (ISMessageBox::ShowQuestion(nullptr, LANG("Message.Question.Reconnect")))
		{
			ISReconnectForm::GetInstance().show();
			ISReconnectForm::GetInstance().StartReconnect(Result);
			if (Result)
			{
				ISMessageBox::ShowInformation(nullptr, LANG("Message.Information.Reconnect.Done"));
			}
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
