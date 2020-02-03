#include "ISStatisticTablesForm.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISScrollArea.h"
#include "ISPushButton.h"
#include "ISTableDetailsForm.h"
//-----------------------------------------------------------------------------
static QString QS_TABLES = PREPARE_QUERY("SELECT tablename "
										 "FROM pg_tables "
										 "WHERE schemaname = current_schema() "
										 "ORDER BY tablename");
//-----------------------------------------------------------------------------
ISStatisticTablesForm::ISStatisticTablesForm(QWidget *parent) : ISInterfaceForm(parent)
{
	QFormLayout *FormLayout = new QFormLayout();
	ISScrollArea *ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(FormLayout);
	GetMainLayout()->addWidget(ScrollArea);

	ISQuery qSelect(QS_TABLES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString TableName = qSelect.ReadColumn("tablename").toString();
			
			ISPushButton *ButtonTable = new ISPushButton(ScrollArea);
			ButtonTable->setText(LANG("Details"));
			ButtonTable->setProperty("TableName", TableName);
			ButtonTable->setSizePolicy(QSizePolicy::Maximum, ButtonTable->sizePolicy().verticalPolicy());
			connect(ButtonTable, &ISPushButton::clicked, this, &ISStatisticTablesForm::DetailsClicked);
			FormLayout->addRow(TableName + ':', ButtonTable);
		}
	}
}
//-----------------------------------------------------------------------------
ISStatisticTablesForm::~ISStatisticTablesForm()
{

}
//-----------------------------------------------------------------------------
void ISStatisticTablesForm::DetailsClicked()
{
	ISTableDetailsForm TableDetailsForm(sender()->property("TableName").toString());
	TableDetailsForm.Exec();
}
//-----------------------------------------------------------------------------
