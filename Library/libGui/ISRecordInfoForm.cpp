#include "ISRecordInfoForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISFieldEditBase.h"
#include "ISQuery.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISRecordInfoForm::ISRecordInfoForm(PMetaClassTable *MetaTable, int ObjectID, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowIcon(BUFFER_ICONS("RecordInformation"));
	setWindowTitle(MetaTable->LocalName);
	ForbidResize();

	QFormLayout *FormLayout = new QFormLayout();
	FormLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(FormLayout);

	QLabel *LabelTitle = new QLabel(this);
	LabelTitle->setText(LANG("SystemInformation") + ':');
	LabelTitle->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelTitle->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetUnderline(LabelTitle, true);
	FormLayout->addRow(LabelTitle, new QWidget(this));

	for (int i = 0; i < MetaTable->SystemFields.count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->SystemFields[i];

		QLabel *Label = new QLabel(this);
		Label->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		Label->setText(MetaField->LabelName + ':');

		ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
		FieldEditBase->SetReadOnly(true);
		FieldEditBase->SetVisibleClear(false);
		FormLayout->addRow(Label, FieldEditBase);

		ISQuery qSelect("SELECT " + MetaTable->Alias + '_' + MetaField->Name + " FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID");
		qSelect.BindValue(":ObjectID", ObjectID);
		if (qSelect.ExecuteFirst())
		{
			QVariant Value = qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name);
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
