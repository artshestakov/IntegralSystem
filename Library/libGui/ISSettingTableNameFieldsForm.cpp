#include "ISSettingTableNameFieldsForm.h"
#include "ISMetaData.h"
#include "ISScrollArea.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_NAME_VALUE = PREPARE_QUERY("SELECT lcnm_localname "
											 "FROM _localnames "
											 "WHERE lcnm_user = currentuserid() "
											 "AND lcnm_tablename = :TableName "
											 "AND lcnm_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QS_NAME = PREPARE_QUERY("SELECT COUNT(*) "
									   "FROM _localnames "
									   "WHERE lcnm_user = currentuserid() "
									   "AND lcnm_tablename = :TableName "
									   "AND lcnm_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QI_NAME = PREPARE_QUERY("INSERT INTO _localnames (lcnm_tablename, lcnm_fieldname, lcnm_localname) "
									   "VALUES (:TableName, :FieldName, :LocalName)");
//-----------------------------------------------------------------------------
static QString QU_NAME = PREPARE_QUERY("UPDATE _localnames SET "
									   "lcnm_localname = :LocalName "
									   "WHERE lcnm_user = currentuserid() "
									   "AND lcnm_tablename = :TableName "
									   "AND lcnm_fieldname = :FieldName");
//-----------------------------------------------------------------------------
ISSettingTableNameFieldsForm::ISSettingTableNameFieldsForm(QWidget *parent) : ISInterfaceForm(parent)
{
	ISScrollArea *ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(new QVBoxLayout());
	GetMainLayout()->addWidget(ScrollArea);

	for (PMetaTable *MetaTable : ISMetaData::GetInstanse().GetTables())
	{
		if (MetaTable->IsSystem)
		{
			continue;
		}

		QFormLayout *FormLayout = new QFormLayout();
		QGroupBox *GroupBox = new QGroupBox(MetaTable->LocalListName, ScrollArea);
		GroupBox->setLayout(FormLayout);
		ScrollArea->widget()->layout()->addWidget(GroupBox);

		for (PMetaField *MetaField : MetaTable->Fields)
		{
			ISLineEdit *LineEdit = new ISLineEdit(GroupBox);
			LineEdit->setSizePolicy(QSizePolicy::Maximum, LineEdit->sizePolicy().verticalPolicy());
			LineEdit->setProperty("TableName", MetaTable->Name);
			LineEdit->setProperty("FieldName", MetaField->Name.toLower());
			FormLayout->addRow(MetaField->LabelName + ":", LineEdit);
			Edits.push_back(LineEdit);

			ISQuery qSelect(QS_NAME_VALUE);
			qSelect.BindValue(":TableName", MetaTable->Name);
			qSelect.BindValue(":FieldName", MetaField->Name.toLower());
			if (qSelect.ExecuteFirst())
			{
				if (qSelect.GetCountResultRows() > 0)
				{
					LineEdit->SetValue(qSelect.ReadColumn("lcnm_localname"));
					LineEdit->SetModificationFlag(false);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISSettingTableNameFieldsForm::~ISSettingTableNameFieldsForm()
{
	for (ISLineEdit *LineEdit : Edits)
	{
		if (LineEdit->GetModificationFlag())
		{
			QString TableName = LineEdit->property("TableName").toString();
			QString FieldName = LineEdit->property("FieldName").toString();
			QVariant Value = LineEdit->GetValue();

			ISQuery qSelect(QS_NAME);
			qSelect.BindValue(":TableName", TableName);
			qSelect.BindValue(":FieldName", FieldName);
			if (qSelect.ExecuteFirst())
			{
				if (qSelect.ReadColumn("count").toBool())
				{
					ISQuery qUpdate(QU_NAME);
					qUpdate.BindValue(":LocalName", Value);
					qUpdate.BindValue(":TableName", TableName);
					qUpdate.BindValue(":FieldName", FieldName);
					qUpdate.Execute();
				}
				else
				{
					if (!Value.toString().isEmpty())
					{
						ISQuery qInsert(QI_NAME);
						qInsert.BindValue(":TableName", TableName);
						qInsert.BindValue(":FieldName", FieldName);
						qInsert.BindValue(":LocalName", Value);
						qInsert.Execute();
					}
				}
				PMetaField *MetaField = ISMetaData::GetInstanse().GetMetaTable(TableName)->GetField(FieldName);
				MetaField->LabelName = Value.toString();
				MetaField->LocalListName = MetaField->LabelName;
			}
		}
	}
}
//-----------------------------------------------------------------------------
