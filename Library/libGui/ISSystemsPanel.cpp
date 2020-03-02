#include "ISSystemsPanel.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISMetaSystemsEntity.h"
#include "ISProtocol.h"
#include "ISObjects.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISSystemsPanel::ISSystemsPanel(QWidget *parent) : QWidget(parent)
{
	Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->setSpacing(0);
	setLayout(Layout);

	SystemsBar = new QToolBar(this);
	SystemsBar->setIconSize(ISDefines::Gui::SIZE_32_32);
	SystemsBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	Layout->addWidget(SystemsBar);

	LineSystems = ISControls::CreateHorizontalLine(this);
	Layout->addWidget(LineSystems);

	QPalette PaletteSystems = LineSystems->palette();
	PaletteSystems.setColor(QPalette::Dark, ISDefines::Gui::COLOR_MAIN_MENU_BAR);
	LineSystems->setPalette(PaletteSystems);

	SubSystemBar = new QToolBar(this);
	SubSystemBar->setVisible(false);
	SubSystemBar->setIconSize(ISDefines::Gui::SIZE_25_25);
	SubSystemBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	Layout->addWidget(SubSystemBar);

	LineSubSystems = ISControls::CreateHorizontalLine(this);
	LineSubSystems->setVisible(false);
	Layout->addWidget(LineSubSystems);

	QPalette PaletteSubSystems = LineSubSystems->palette();
	PaletteSubSystems.setColor(QPalette::Dark, ISDefines::Gui::COLOR_MAIN_MENU_BAR);
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
	QAction *ActionSystem = SystemsBar->addAction(GetSystemIcon(MetaSystem), MetaSystem->LocalName, this, &ISSystemsPanel::SystemClicked);
	ActionSystem->setToolTip(GetSystemToolTip(MetaSystem));
	ActionSystem->setFont(ISDefines::Gui::FONT_TAHOMA_15);
	ActionSystem->setCheckable(true);
	SystemsBar->widgetForAction(ActionSystem)->setCursor(CURSOR_POINTING_HAND);

	ActionSystemGroup->addAction(ActionSystem);
	Systems.insert(ActionSystem, MetaSystem);
}
//-----------------------------------------------------------------------------
void ISSystemsPanel::SystemClicked()
{
	QAction *ActionClicked = dynamic_cast<QAction*>(sender());
	QString ClickedSystemUID = Systems.value(ActionClicked)->UID;
	if (CurrentSystemUID == ClickedSystemUID)
	{
		return;
	}
	else
	{
		CurrentSystemUID = ClickedSystemUID;
	}

	ISGui::SetWaitGlobalCursor(true);
	ClearSubSystemsBar();

	SubSystemBar->setVisible(true);
	LineSubSystems->setVisible(true);

	LoadSubSystemsBar(Systems.value(ActionClicked));
	ISProtocol::Insert(true, CONST_UID_PROTOCOL_OPEN_SYSTEM, QString(), QString(), QVariant(), Systems.value(ActionClicked)->LocalName);
	ISGui::SetWaitGlobalCursor(false);
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
	for (ISMetaSubSystem *MetaSubSystem : MetaSystem->SubSystems)
	{
		QAction *ActionSubSystem = SubSystemBar->addAction(GetSubSystemIcon(MetaSubSystem), MetaSubSystem->LocalName, this, &ISSystemsPanel::SubSystemClicked);
		ActionSubSystem->setObjectName(MetaSubSystem->UID);
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
	QString ToolTip = MetaSystem->Hint.isEmpty() ? MetaSystem->LocalName : MetaSystem->Hint;
	ToolTip = ISGui::PrepareLongToolTip(ToolTip);
	return ToolTip;
}
//-----------------------------------------------------------------------------
QString ISSystemsPanel::GetSubSystemToolTip(ISMetaSubSystem *MetaSubSystem) const
{
	QString ToolTip = MetaSubSystem->Hint.isEmpty() ? MetaSubSystem->LocalName : MetaSubSystem->Hint;
	ToolTip = ISGui::PrepareLongToolTip(ToolTip);
	return ToolTip;
}
//-----------------------------------------------------------------------------
QIcon ISSystemsPanel::GetSystemIcon(ISMetaSystem *MetaSystem) const
{
	return MetaSystem->IsSystem ? BUFFER_ICONS(MetaSystem->IconName) : ISObjects::GetInstance().GetInterface()->GetIcon(MetaSystem->IconName);
}
//-----------------------------------------------------------------------------
QIcon ISSystemsPanel::GetSubSystemIcon(ISMetaSubSystem *MetaSubSystem) const
{
	return ISMetaSystemsEntity::GetInstance().GetSystem(MetaSubSystem->SystemUID)->IsSystem ?
		BUFFER_ICONS(MetaSubSystem->IconName) :
		ISObjects::GetInstance().GetInterface()->GetIcon(MetaSubSystem->IconName);
}
//-----------------------------------------------------------------------------
