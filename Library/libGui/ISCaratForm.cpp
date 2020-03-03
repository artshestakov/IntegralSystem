#include "ISCaratForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISCheckEdit.h"
#include "ISGui.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_uid, core_localname, core_active "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
static QString QU_CARAT_ACTIVE = PREPARE_QUERY("UPDATE _caratcore SET "
											   "core_active = :Active "
											   "WHERE core_uid = :UID");
//-----------------------------------------------------------------------------
ISCaratForm::ISCaratForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QFormLayout *FormLayout = new QFormLayout();

	QGroupBox *GroupBoxCores = new QGroupBox(this);
	GroupBoxCores->setTitle(LANG("Cores"));
	GroupBoxCores->setLayout(FormLayout);
	Layout->addWidget(GroupBoxCores);

	ISQuery qSelect(QS_CARAT_CORE);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid CoreUID = qSelect.ReadColumn("core_uid");
			QString CoreName = qSelect.ReadColumn("core_localname").toString();
			bool CoreActive = qSelect.ReadColumn("core_active").toBool();

			ISCheckEdit *CheckBox = new ISCheckEdit(GroupBoxCores);
			CheckBox->SetValue(CoreActive);
			CheckBox->SetToolTip(LANG("ClickFromIncludeAndExcludeCore"));
			CheckBox->setObjectName(CoreUID);
			connect(CheckBox, &ISCheckEdit::ValueChange, this, &ISCaratForm::CoreChecked);
			FormLayout->addRow(CoreName + ':', CheckBox);
		}
	}
}
//-----------------------------------------------------------------------------
ISCaratForm::~ISCaratForm()
{

}
//-----------------------------------------------------------------------------
void ISCaratForm::LoadData()
{

}
//-----------------------------------------------------------------------------
void ISCaratForm::CoreChecked(const QVariant &value	)
{
	ISGui::SetWaitGlobalCursor(true);
	ISQuery qUpdate(QU_CARAT_ACTIVE);
	qUpdate.BindValue(":Active", dynamic_cast<ISCheckEdit*>(sender())->GetValue().toBool());
	qUpdate.BindValue(":UID", sender()->objectName());
	bool Result = qUpdate.Execute();
	ISGui::SetWaitGlobalCursor(false);

	if (!Result)
	{
		ISMessageBox::ShowCritical(this, qUpdate.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
