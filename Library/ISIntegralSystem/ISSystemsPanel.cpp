#include "StdAfx.h"
#include "ISSystemsPanel.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISUserRoleEntity.h"
#include "ISMetaSystemsEntity.h"
#include "ISSystem.h"
#include "ISProtocol.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
ISSystemsPanel::ISSystemsPanel(QWidget *parent) : QWidget(parent)
{
	Layout = new QVBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_NULL);
	Layout->setSpacing(0);
	setLayout(Layout);

	SystemsBar = new QToolBar(this);
	SystemsBar->setIconSize(SIZE_SYSTEM_ICON);
	SystemsBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	Layout->addWidget(SystemsBar);

	LineSystems = ISControls::CreateHorizontalLine(this);
	Layout->addWidget(LineSystems);

	QPalette PaletteSystems = LineSystems->palette();
	PaletteSystems.setColor(QPalette::Dark, COLOR_MAIN_MENU_BAR);
	LineSystems->setPalette(PaletteSystems);

	SubSystemBar = new QToolBar(this);
	SubSystemBar->setVisible(false);
	SubSystemBar->setIconSize(SIZE_25_25);
	SubSystemBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	Layout->addWidget(SubSystemBar);

	LineSubSystems = ISControls::CreateHorizontalLine(this);
	LineSubSystems->setVisible(false);
	Layout->addWidget(LineSubSystems);

	QPalette PaletteSubSystems = LineSubSystems->palette();
	PaletteSubSystems.setColor(QPalette::Dark, COLOR_MAIN_MENU_BAR);
	LineSubSystems->setPalette(PaletteSubSystems);

	ActionSystemGroup = new QActionGroup(this);
	ActionSubSystemGroup = new QActionGroup(this);
}
//-----------------------------------------------------------------------------
ISSystemsPanel::~ISSystemsPanel()
{

}
//-----------------------------------------------------------------------------
void ISSystemsPanel::AddSystem(ISMetaSystem *MetaSystem)
{
	QAction *ActionSystem = SystemsBar->addAction(GetSystemIcon(MetaSystem), MetaSystem->GetLocalName(), this, &ISSystemsPanel::SystemClicked);
	ActionSystem->setToolTip(GetSystemToolTip(MetaSystem));
	ActionSystem->setFont(FONT_TAHOMA_15);
	ActionSystem->setCheckable(true);
	SystemsBar->widgetForAction(ActionSystem)->setCursor(CURSOR_POINTING_HAND);

	ActionSystemGroup->addAction(ActionSystem);
	Systems.insert(ActionSystem, MetaSystem);
}
//-----------------------------------------------------------------------------
void ISSystemsPanel::SystemClicked()
{
	QAction *ActionClicked = dynamic_cast<QAction*>(sender());
	QString ClickedSystemUID = Systems.value(ActionClicked)->GetUID();
	if (CurrentSystemUID == ClickedSystemUID)
	{
		return;
	}
	else
	{
		CurrentSystemUID = ClickedSystemUID;
	}

	ISSystem::SetWaitGlobalCursor(true);
	ClearSubSystemsBar();

	SubSystemBar->setVisible(true);
	LineSubSystems->setVisible(true);

	LoadSubSystemsBar(Systems.value(ActionClicked));
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_OPEN_SYSTEM, QString(), QString(), QVariant(), Systems.value(ActionClicked)->GetLocalName());
	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISSystemsPanel::SubSystemClicked()
{
	QAction *ActionClicked = dynamic_cast<QAction*>(sender());

	CurrentSubSystemUID = ActionClicked->objectName();
	emit ClickedSubSystem(CurrentSubSystemUID, ActionClicked->icon());
}
//-----------------------------------------------------------------------------
void ISSystemsPanel::LoadSubSystemsBar(ISMetaSystem *MetaSystem)
{
	for (ISMetaSubSystem *MetaSubSystem : MetaSystem->GetSubSystems())
	{
		QAction *ActionSubSystem = SubSystemBar->addAction(GetSubSystemIcon(MetaSubSystem), MetaSubSystem->GetLocalName(), this, &ISSystemsPanel::SubSystemClicked);
		ActionSubSystem->setObjectName(MetaSubSystem->GetUID());
		ActionSubSystem->setToolTip(GetSubSystemToolTip(MetaSubSystem));
		ActionSubSystem->setCheckable(true);

		SubSystemBar->widgetForAction(ActionSubSystem)->setCursor(CURSOR_POINTING_HAND);
		ActionSubSystemGroup->addAction(ActionSubSystem);

		if (ActionSubSystem->objectName() == CurrentSubSystemUID)
		{
			ActionSubSystem->setChecked(true);
		}
	}
}
//-----------------------------------------------------------------------------
void ISSystemsPanel::ClearSubSystemsBar()
{
	while (SubSystemBar->actions().count()) //Обход действий подсистем
	{
		QAction *Action = SubSystemBar->actions().takeFirst(); //Первый элемент списка

		if (ActionSubSystemGroup->actions().contains(Action))
		{
			ActionSubSystemGroup->removeAction(Action);
		}

		SubSystemBar->actions().removeAll(Action);
		delete Action;
		Action = nullptr;
	}
}
//-----------------------------------------------------------------------------
QString ISSystemsPanel::GetSystemToolTip(ISMetaSystem *MetaSystem) const
{
	QString ToolTip;

	if (MetaSystem->GetHint().length())
	{
		ToolTip = MetaSystem->GetHint();
	}
	else
	{
		ToolTip = MetaSystem->GetLocalName();
	}

	ToolTip = ISSystem::PrepareLongToolTip(ToolTip);

	return ToolTip;
}
//-----------------------------------------------------------------------------
QString ISSystemsPanel::GetSubSystemToolTip(ISMetaSubSystem *MetaSubSystem) const
{
	QString ToolTip;

	if (MetaSubSystem->GetHint().length())
	{
		ToolTip = MetaSubSystem->GetHint();
	}
	else
	{
		ToolTip = MetaSubSystem->GetLocalName();
	}

	ToolTip = ISSystem::PrepareLongToolTip(ToolTip);

	return ToolTip;
}
//-----------------------------------------------------------------------------
QIcon ISSystemsPanel::GetSystemIcon(ISMetaSystem *MetaSystem) const
{
	QIcon IconSystem;

	if (MetaSystem->GetIsSystem()) //Система является движковой
	{
		IconSystem = BUFFER_ICONS(MetaSystem->GetIconName());
	}
	else //Система является пользовательской (т.е. из конфигурации)
	{
		IconSystem = ISObjects::GetInstance().GetInterface()->GetIcon(MetaSystem->GetIconName());
	}

	return IconSystem;
}
//-----------------------------------------------------------------------------
QIcon ISSystemsPanel::GetSubSystemIcon(ISMetaSubSystem *MetaSubSystem) const
{
	QIcon IconSubSystem;

	if (ISMetaSystemsEntity::GetInstance().GetSystem(MetaSubSystem->GetSystemUID())->GetIsSystem())
	{
		IconSubSystem = BUFFER_ICONS(MetaSubSystem->GetIconName());
	}
	else
	{
		IconSubSystem = ISObjects::GetInstance().GetInterface()->GetIcon(MetaSubSystem->GetIconName());
	}

	return IconSubSystem;
}
//-----------------------------------------------------------------------------
