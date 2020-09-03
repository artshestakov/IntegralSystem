#include "ISSettingsDatabaseListForm.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QU_ACTIVE = PREPARE_QUERY("SELECT set_setting_database_active(:ID)");
//-----------------------------------------------------------------------------
ISSettingsDatabaseListForm::ISSettingsDatabaseListForm(QWidget *parent) : ISListBaseForm("_SettingsDatabase", parent)
{
	QAction *ActionActive = new QAction(BUFFER_ICONS("Apply.Blue"), LANG("ActiveProfile"), GetToolBar());
	connect(ActionActive, &QAction::triggered, this, &ISSettingsDatabaseListForm::Active);
	AddAction(ActionActive, true, true);
}
//-----------------------------------------------------------------------------
ISSettingsDatabaseListForm::~ISSettingsDatabaseListForm()
{

}
//-----------------------------------------------------------------------------
void ISSettingsDatabaseListForm::Active()
{
	ISQuery qUpdate(QU_ACTIVE);
	qUpdate.BindValue(":ID", GetObjectID());
	qUpdate.Execute() ? Update() : ISMessageBox::ShowWarning(this, LANG("Message.Warning.ActiveSettingDatabase"), qUpdate.GetErrorString());
}
//-----------------------------------------------------------------------------
