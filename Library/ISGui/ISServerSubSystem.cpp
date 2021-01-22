#include "ISServerSubSystem.h"
#include "ISLocalization.h"
#include "ISTcpQuery.h"
#include "ISFieldEdits.h"
#include "ISDialogsCommon.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISServerSubSystem::ISServerSubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	TabWidget = new QTabWidget(this);
	TabWidget->setDocumentMode(false);
	GetMainLayout()->addWidget(TabWidget);
}
//-----------------------------------------------------------------------------
ISServerSubSystem::~ISServerSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISServerSubSystem::LoadData()
{
	CreateTabLog();
}
//-----------------------------------------------------------------------------
void ISServerSubSystem::CreateTabLog()
{
	QVBoxLayout *LayoutWidget = new QVBoxLayout();

	QWidget *Widget = new QWidget(TabWidget);
	Widget->setLayout(LayoutWidget);
	TabWidget->addTab(Widget, LANG("ISServerSubSystem.Title"));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	LayoutWidget->addLayout(LayoutTitle);

	LayoutTitle->addWidget(new QLabel(LANG("ISServerSubSystem.LogView.Date"), Widget));

	ISDateEdit *DateEdit = new ISDateEdit(Widget);
	connect(DateEdit, &ISDateEdit::ValueChange, this, &ISServerSubSystem::TabLogDateChanged);
	LayoutTitle->addWidget(DateEdit);

	LayoutTitle->addStretch();

	TableView = new ISBaseTableView(Widget);
	TableView->verticalHeader()->setVisible(false);
	LayoutWidget->addWidget(TableView);

	LogModel = new ISLogModel(TableView);
	TableView->setModel(LogModel);

	LabelBottom = new QLabel(LANG("ISServerSubSystem.LogView.RowCount").arg(0), Widget);
	LayoutWidget->addWidget(LabelBottom, 0, Qt::AlignLeft);
}
//-----------------------------------------------------------------------------
void ISServerSubSystem::TabLogDateChanged(const QVariant &Date)
{
	ISTcpQuery qLogGet(API_LOG_GET);
	qLogGet.BindValue("Date", Date.toDate().toString(FORMAT_DATE_V2));

	ISGui::SetWaitGlobalCursor(true);
	if (qLogGet.Execute())
	{
		QVariantMap AnswerMap = qLogGet.TakeAnswer();
		LogModel->SetSource(AnswerMap["FieldList"].toStringList(), AnswerMap["RecordList"].toList());
		TableView->resizeColumnsToContents();
		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, qLogGet.GetErrorString());
	}
	LabelBottom->setText(LANG("ISServerSubSystem.LogView.RowCount")
		.arg(LogModel->rowCount())
		.arg(LogModel->GetCountDebug())
		.arg(LogModel->GetCountInfo())
		.arg(LogModel->GetCountWarning())
		.arg(LogModel->GetCountError())
		.arg(LogModel->GetCountCritical())
		.arg(LogModel->GetCountTrace())
		.arg(LogModel->GetCountAssert()));
}
//-----------------------------------------------------------------------------
