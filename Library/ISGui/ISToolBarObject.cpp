#include "ISToolBarObject.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISToolBarObject::ISToolBarObject(QWidget *parent) : QToolBar(parent)
{
	CurrentAction = nullptr;

	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	
	connect(this, &QToolBar::actionTriggered, this, &ISToolBarObject::ActionTriggered);

	ActionGroup = new QActionGroup(this);
}
//-----------------------------------------------------------------------------
ISToolBarObject::~ISToolBarObject()
{

}
//-----------------------------------------------------------------------------
QAction* ISToolBarObject::GetCurrentAction()
{
	return CurrentAction;
}
//-----------------------------------------------------------------------------
QAction* ISToolBarObject::CreateAction(const QIcon &Icon, const QString &Text, ISNamespace::ObjectActionType action_type, const QString &TableName, const QString &ClassName)
{
	QAction *Action = new QAction(this);
	Action->setText(Text);
	Action->setToolTip(Text);
	Action->setIcon(Icon);
	Action->setCheckable(true);
	Action->setProperty("Type", action_type);
	Action->setProperty("TableName", TableName);
	Action->setProperty("ClassName", ClassName);
	ActionGroup->addAction(Action);
	
	return Action;
}
//-----------------------------------------------------------------------------
void ISToolBarObject::UpdateEnabledActionsList(bool Enabled)
{
	for (QAction *Action : actions())
	{
		if (!Action->property("Object").toBool())
		{
			Action->setEnabled(Enabled);
		}
	}
}
//-----------------------------------------------------------------------------
void ISToolBarObject::ActionTriggered(QAction *ActionTriggered)
{
	for (QAction *Action : actions())
	{
		Action->setFont(ISDefines::Gui::FONT_APPLICATION);
	}

	if (qvariant_cast<ISNamespace::ObjectActionType>(ActionTriggered->property("Type")) != ISNamespace::OAT_Service)
	{
		ActionTriggered->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	}

	if (CurrentAction != ActionTriggered)
	{
		CurrentAction = ActionTriggered;
		emit ActionClicked(ActionTriggered);
	}
}
//-----------------------------------------------------------------------------
