#include "StdAfx.h"
#include "ISRecordInfoForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISDefines.h"
#include "ISFieldEditBase.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISRecordInfoForm::ISRecordInfoForm(PMetaClassTable *MetaTable, int ObjectID, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowIcon(BUFFER_ICONS("RecordInformation"));
	setWindowTitle(MetaTable->GetLocalName());
	ForbidResize();

	QFormLayout *FormLayout = new QFormLayout();
	FormLayout->setContentsMargins(LAYOUT_MARGINS_10_PX);
	GetMainLayout()->addLayout(FormLayout);

	QLabel *LabelTitle = new QLabel(this);
	LabelTitle->setText(LANG("SystemInformation") + ":");
	LabelTitle->setFont(FONT_TAHOMA_12_BOLD);
	LabelTitle->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetUnderline(LabelTitle, true);
	FormLayout->addRow(LabelTitle, new QWidget(this));

	for (int i = 0; i < MetaTable->GetSystemFields().count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->GetSystemFields().at(i);

		QLabel *Label = new QLabel(this);
		Label->setFont(FONT_APPLICATION_BOLD);
		Label->setText(MetaField->GetLabelName() + ":");

		ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
		FieldEditBase->SetReadOnly(true);
		FieldEditBase->SetVisibleClear(false);
		FormLayout->addRow(Label, FieldEditBase);

		ISQuery qSelect("SELECT " + MetaTable->GetAlias() + '_' + MetaField->GetName() + " FROM " + MetaTable->GetName() + " WHERE " + MetaTable->GetAlias() + "_id = :ObjectID");
		qSelect.BindValue(":ObjectID", ObjectID);
		if (qSelect.ExecuteFirst())
		{
			QVariant Value = qSelect.ReadColumn(MetaTable->GetAlias() + '_' + MetaField->GetName());
			FieldEditBase->SetValue(Value);
		}
	}

	setFocus();
}
//-----------------------------------------------------------------------------
ISRecordInfoForm::~ISRecordInfoForm()
{

}
//-----------------------------------------------------------------------------
