#include "ISIntegralSystem.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISReconnectForm.h"
#include "ISAutoLocking.h"
#include "ISGui.h"
#include "ISException.h"
//-----------------------------------------------------------------------------
ISIntegralSystem::ISIntegralSystem(int &argc, char **argv) : QApplication(argc,	argv)
{
	setEffectEnabled(Qt::UI_General);
	setEffectEnabled(Qt::UI_AnimateMenu);
	setEffectEnabled(Qt::UI_FadeMenu);
	setEffectEnabled(Qt::UI_AnimateCombo);
	setEffectEnabled(Qt::UI_AnimateTooltip);
	setEffectEnabled(Qt::UI_FadeTooltip);
	setEffectEnabled(Qt::UI_AnimateToolBox);
}
//-----------------------------------------------------------------------------
ISIntegralSystem::~ISIntegralSystem()
{
	
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
	catch (const ISExceptionSqlSyntax &ExceptionSyntax)
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.QueryExceptionSyntax"), ExceptionSyntax.What());
	}
	catch (const ISExceptionConnectionDB &QueryExceptionConnection)
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
