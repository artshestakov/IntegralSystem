#include "StdAfx.h"
#include "ISCaratForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISCheckEdit.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_uid, core_localname, crca_active "
											 "FROM _caratcore "
											 "LEFT JOIN _caratcoreactive ON crca_core = core_uid "
											 "WHERE NOT core_isdeleted "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
static QString QS_CARAT_ACTIVE_COUNT = PREPARE_QUERY("SELECT COUNT(*) "
													 "FROM _caratcoreactive "
													 "WHERE crca_core = :CoreUID");
//-----------------------------------------------------------------------------
static QString QI_CARAT_CORE_ACTIVE = PREPARE_QUERY("INSERT INTO _caratcoreactive(crca_core, crca_active) "
													"VALUES(:CoreUID, :Active)");
//-----------------------------------------------------------------------------
static QString QU_CARAT_CORE_ACTIVE = PREPARE_QUERY("UPDATE _caratcoreactive SET "
													"crca_active = :Active "
													"WHERE crca_core = :CoreUID");
//-----------------------------------------------------------------------------
ISCaratForm::ISCaratForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QFormLayout *FormLayout = new QFormLayout();

	QGroupBox *GroupBoxCores = new QGroupBox(this);
	GroupBoxCores->setTitle(LOCALIZATION("Cores"));
	GroupBoxCores->setLayout(FormLayout);
	Layout->addWidget(GroupBoxCores);

	ISQuery qSelect(QS_CARAT_CORE);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid CoreUID = qSelect.ReadColumn("core_uid");
			QString CoreName = qSelect.ReadColumn("core_localname").toString();
			bool CoreActive = qSelect.ReadColumn("crca_active").toBool();

			ISCheckEdit *CheckBox = new ISCheckEdit(GroupBoxCores);
			CheckBox->SetValue(CoreActive);
			CheckBox->SetToolTip(LOCALIZATION("ClickFromIncludeAndExcludeCore"));
			CheckBox->setObjectName(CoreUID);
			connect(CheckBox, &ISCheckEdit::ValueChange, this, &ISCaratForm::CoreChecked);
			FormLayout->addRow(CoreName + ":", CheckBox);
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

	ISUuid CoreUID = sender()->objectName();

	ISQuery qSelectCount(QS_CARAT_ACTIVE_COUNT);
	qSelectCount.BindValue(":CoreUID", CoreUID);
	if (qSelectCount.ExecuteFirst())
	{
		int Count = qSelectCount.ReadColumn("count").toInt();
		if (Count)
		{
			ISQuery qUpdate(QU_CARAT_CORE_ACTIVE);
			qUpdate.BindValue(":Active", value);
			qUpdate.BindValue(":CoreUID", CoreUID);
			qUpdate.Execute();
		}
		else
		{
			ISQuery qInsert(QI_CARAT_CORE_ACTIVE);
			qInsert.BindValue(":CoreUID", CoreUID);
			qInsert.BindValue(":Active", value);
			qInsert.Execute();
		}
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
