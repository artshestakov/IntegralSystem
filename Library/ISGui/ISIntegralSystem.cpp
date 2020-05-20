#include "ISIntegralSystem.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISReconnectForm.h"
#include "ISGui.h"
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
	return QApplication::notify(Object, e);
}
//-----------------------------------------------------------------------------
