#include "ISQuerySubSystem.h"
#include "ISDefinesGui.h"
#include "ISTcpQuery.h"
#include "ISDialogsCommon.h"
#include "ISLocalization.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISQuerySubSystem::ISQuerySubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	ListWidgetQuery = new ISListWidget(this);
	ListWidgetQuery->setAlternatingRowColors(true);
	connect(ListWidgetQuery, &ISListWidget::itemClicked, this, &ISQuerySubSystem::ItemClicked);
	GetMainLayout()->addWidget(ListWidgetQuery);

	TextEdit = new ISTextEdit(this);
	TextEdit->SetReadOnly(true);
	GetMainLayout()->addWidget(TextEdit);
}
//-----------------------------------------------------------------------------
ISQuerySubSystem::~ISQuerySubSystem()
{

}
//-----------------------------------------------------------------------------
void ISQuerySubSystem::LoadData()
{
	ISTcpQuery qGetStatement(API_GET_STATEMENT);

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qGetStatement.Execute();
	ISGui::SetWaitGlobalCursor(false);
	if (!Result)
	{
		ISMessageBox::ShowCritical(this, qGetStatement.GetErrorString());
		return;
	}

	QVariantMap AnswerMap = qGetStatement.TakeAnswer();
	QVariantList QueryList = AnswerMap["QueryList"].toList();

	for (const QVariant &Variant : QueryList)
	{
		QVariantMap QueryMap = Variant.toMap();

		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetQuery);
		ListWidgetItem->setText(LANG("ISQuerySubSystem.ItemText").
			arg(QueryMap["UserID"].toString()).
			arg(QueryMap["Login"].toString()).
			arg(QueryMap["Calls"].toString()).
			arg(QueryMap["MSec"].toString()));
		ListWidgetItem->setData(Qt::UserRole, QueryMap["SqlQuery"]);
	}
}
//-----------------------------------------------------------------------------
void ISQuerySubSystem::ItemClicked(QListWidgetItem *ListWidgetItem)
{
	TextEdit->SetValue(ListWidgetItem->data(Qt::UserRole));
}
//-----------------------------------------------------------------------------
