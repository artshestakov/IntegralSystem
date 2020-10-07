#include "ISRecordInfoForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISFieldEditBase.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISCore.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISRecordInfoForm::ISRecordInfoForm(PMetaTable *MetaTable, int ObjectID) : ISInterfaceDialogForm()
{
	setWindowIcon(BUFFER_ICONS("RecordInformation"));
	setWindowTitle(LANG("SystemInformation").arg(MetaTable->LocalName));
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	
	QFormLayout *FormLayout = new QFormLayout();
	FormLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	GetMainLayout()->addLayout(FormLayout);

	FormLayout->addRow(new QLabel(LANG("Named") + ':', this), new QLabel(ISCore::GetObjectName(MetaTable, ObjectID), this));
	FormLayout->addWidget(ISControls::CreateHorizontalLine(this));

	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (!MetaField->IsSystem)
		{
			continue;
		}

		QLabel *Label = new QLabel(this);
		Label->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		Label->setText(MetaField->LabelName + ':');

		ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(this, MetaField);
		FieldEditBase->SetReadOnly(true);
		FormLayout->addRow(Label, FieldEditBase);

		ISQuery qSelect("SELECT " + MetaTable->Alias + '_' + MetaField->Name + " FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID");
		qSelect.BindValue(":ObjectID", ObjectID);
		if (qSelect.ExecuteFirst())
		{
			QVariant Value = qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name);
			if (MetaField->Foreign)
			{
				dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
			}
			FieldEditBase->SetValue(Value);
		}
	}
	
	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISRecordInfoForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISRecordInfoForm::~ISRecordInfoForm()
{

}
//-----------------------------------------------------------------------------
