#include "ISServerLogsSubSystem.h"
#include "ISLocalization.h"
#include "ISTcpQuery.h"
#include "ISDialogsCommon.h"
#include "ISGui.h"
#include "ISDelegates.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISServerLogsSubSystem::ISServerLogsSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	TreeWidget = new QTreeWidget(this);
	TreeWidget->header()->setVisible(false);
	TreeWidget->setItemDelegate(new ISPopupDelegate(TreeWidget));
	TreeWidget->setAnimated(true);
	TreeWidget->setAlternatingRowColors(true);
	TreeWidget->setFrameShape(QFrame::NoFrame);
	TreeWidget->setSizePolicy(QSizePolicy::Maximum, TreeWidget->sizePolicy().verticalPolicy());
	connect(TreeWidget, &QTreeWidget::itemDoubleClicked, this, &ISServerLogsSubSystem::ItemDoubleClicked);
	Layout->addWidget(TreeWidget);

	QVBoxLayout *LayoutGroupBox = new QVBoxLayout();
	LayoutGroupBox->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QGroupBox *GroupBox = new QGroupBox(LANG("ISServerLogsSubSystem.Content"), this);
	GroupBox->setLayout(LayoutGroupBox);
	Layout->addWidget(GroupBox);

	TextEdit = new QTextEdit(this);
	TextEdit->setReadOnly(true);
	TextEdit->setFrameShape(QFrame::NoFrame);
	GroupBox->layout()->addWidget(TextEdit);
}
//-----------------------------------------------------------------------------
ISServerLogsSubSystem::~ISServerLogsSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISServerLogsSubSystem::LoadData()
{
	ISTcpQuery qLogGetStructure(API_LOG_GET_STRUCTURE);
	if (!qLogGetStructure.Execute())
	{
		ISMessageBox::ShowCritical(this, qLogGetStructure.GetErrorString());
		return;
	}
	
	QVariantMap YearMap = qLogGetStructure.TakeAnswer()["Years"].toMap();
	for (const auto &YearItem : YearMap.toStdMap())
	{
		QTreeWidgetItem *TreeItemYear = new QTreeWidgetItem(TreeWidget);
		QVariantMap MonthMap = YearItem.second.toMap();
		for (const auto &MonthItem : MonthMap.toStdMap())
		{
			QTreeWidgetItem *TreeItemMonth = new QTreeWidgetItem(TreeItemYear);
			QVariantList DayList = MonthItem.second.toList();
			for (const QVariant &DayName : DayList)
			{
				QTreeWidgetItem *TreeItemDay = new QTreeWidgetItem(TreeItemMonth);
				TreeItemDay->setText(0, DayName.toString());
			}
			TreeItemMonth->setText(0, MonthItem.first + QString(" (%1)").arg(DayList.size()));
		}
		TreeItemYear->setText(0, YearItem.first + QString(" (%1)").arg(MonthMap.size()));
	}
}
//-----------------------------------------------------------------------------
void ISServerLogsSubSystem::ItemDoubleClicked(QTreeWidgetItem *TreeWidgetItem, int Column)
{
	Q_UNUSED(Column);

	if (TreeWidgetItem->childCount() != 0)
	{
		return;
	}

	QStringList StringList = TreeWidgetItem->text(0).split('.');

	ISTcpQuery qLogGetContent(API_LOG_GET_CONTENT);
	qLogGetContent.BindValue("Day", StringList.takeFirst().toUInt());
	qLogGetContent.BindValue("Month", StringList.takeFirst().toUInt());
	qLogGetContent.BindValue("Year", StringList.takeFirst().toUInt());

	ISGui::SetWaitGlobalCursor(true);
	bool Result = qLogGetContent.Execute();
	ISGui::SetWaitGlobalCursor(false);

	Result ? TextEdit->setText(qLogGetContent.TakeAnswer()["Content"].toString()) :
		ISMessageBox::ShowCritical(this, qLogGetContent.GetErrorString());
}
//-----------------------------------------------------------------------------
