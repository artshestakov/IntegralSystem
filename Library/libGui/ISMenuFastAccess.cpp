#include "ISMenuFastAccess.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISGui.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISMenuFastAccess::ISMenuFastAccess(QWidget *parent) : QMenu(parent)
{
	ActionGroupRecords = new QActionGroup(this);
	connect(ActionGroupRecords, &QActionGroup::triggered, this, &ISMenuFastAccess::CreateRecord);

	ActionGroupTools = new QActionGroup(this);
	connect(ActionGroupTools, &QActionGroup::triggered, this, &ISMenuFastAccess::StartExternalTool);

	QLabel *LabelCreateRecords = new QLabel(LANG("CreateRecords") + ':', this);
	ISGui::SetFontWidgetUnderline(LabelCreateRecords, true);
	LabelCreateRecords->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	QWidgetAction *WidgetActionCreateRecords = new QWidgetAction(this);
	WidgetActionCreateRecords->setDefaultWidget(LabelCreateRecords);
	addAction(WidgetActionCreateRecords);

	ActionCreateRecords = addAction(LANG("Setting"), this, &ISMenuFastAccess::CreateRecords);

	QLabel *LabelExternalTools = new QLabel(LANG("ExternalTools") + ':', this);
	ISGui::SetFontWidgetUnderline(LabelExternalTools, true);
	LabelExternalTools->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	QWidgetAction *WidgetActionExternalTools = new QWidgetAction(this);
	WidgetActionExternalTools->setDefaultWidget(LabelExternalTools);
	addAction(WidgetActionExternalTools);

	ActionExternalTools = addAction(LANG("Setting"), this, &ISMenuFastAccess::ExternalTools);

	connect(this, &ISMenuFastAccess::aboutToShow, this, &ISMenuFastAccess::AboutToShow);
}
//-----------------------------------------------------------------------------
ISMenuFastAccess::~ISMenuFastAccess()
{

}
//-----------------------------------------------------------------------------
void ISMenuFastAccess::AboutToShow()
{
	//Очистка быстрого создания записей
	while (ActionGroupRecords->actions().count())
	{
		QAction *Action = ActionGroupRecords->actions().takeFirst();
		removeAction(Action);

		delete Action;
		Action = nullptr;
	}

	//Заполнение быстрого создания записей
	QVectorString VectorRecords = ISFastAccessEntity::GetInstance().GetCreateRecords();
	for (int i = 0; i < VectorRecords.count(); ++i)
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(VectorRecords.at(i));

		QAction *ActionRecord = new QAction(LANG("CreateRecord") + ": " + MetaTable->GetLocalName(), this);
		ActionRecord->setFont(FONT_TAHOMA_10);
		ActionRecord->setData(MetaTable->GetName());
		insertAction(ActionCreateRecords, ActionRecord);
		ActionGroupRecords->addAction(ActionRecord);
	}

	//Очистка внешних инструментов
	while (ActionGroupTools->actions().count())
	{
		QAction *Action = ActionGroupTools->actions().takeFirst();
		removeAction(Action);

		delete Action;
		Action = nullptr;
	}

	//Заполнение внешних инструментов
	QVector<ISMetaExternalTool*> VectorTools = ISFastAccessEntity::GetInstance().GetExternalTools();
	for (int i = 0; i < VectorTools.count(); ++i)
	{
		ISMetaExternalTool *MetaExternalTool = VectorTools.at(i);

		QAction *ActionExternalTool = new QAction(MetaExternalTool->GetIcon(), MetaExternalTool->GetLocalName(), this);
		ActionExternalTool->setFont(FONT_TAHOMA_10);
		ActionExternalTool->setData(MetaExternalTool->GetCommand());
		insertAction(ActionExternalTools, ActionExternalTool);
		ActionGroupTools->addAction(ActionExternalTool);
	}
}
//-----------------------------------------------------------------------------
void ISMenuFastAccess::CreateRecord(QAction *ActionTriggered)
{
	ISGui::CreateObjectForm(ISNamespace::OFT_New, ActionTriggered->data().toString())->show();
}
//-----------------------------------------------------------------------------
void ISMenuFastAccess::StartExternalTool(QAction *ActionTriggered)
{
	QString Command = ActionTriggered->data().toString();

	if (!QFile::exists(Command))
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Error.NotFoundFile").arg(Command));
		return;
	}

	if (!ISGui::OpenFile(Command))
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.NotStartedExternalTool").arg(ActionTriggered->text()));
	}
}
//-----------------------------------------------------------------------------
