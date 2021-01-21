#include "ISSettingsDatabaseObjectForm.h"
#include "ISSettingsDatabase.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISDialogsCommon.h"
//-----------------------------------------------------------------------------
ISSettingsDatabaseObjectForm::ISSettingsDatabaseObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) :
	ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	SetVisibleNavigationBar(false);
	SetVisibleFavorites(false);
	SetVisibleDelete(false);
	SetVisibleNote(false);

	QAction *ActionDefault = new QAction(BUFFER_ICONS("DatabaseSettings"), LANG("ResetSettingsDB"), this);
	connect(ActionDefault, &QAction::triggered, this, &ISSettingsDatabaseObjectForm::ResetSettings);
	AddActionToolBar(ActionDefault, true);
}
//-----------------------------------------------------------------------------
ISSettingsDatabaseObjectForm::~ISSettingsDatabaseObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISSettingsDatabaseObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result) //Сохранение прошло успешно - обновляем значения в памяти
	{
		for (PMetaField *MetaField : GetMetaTable()->Fields) //Обходим все поля
		{
			if (MetaField->IsSystem) //Кроме системных
			{
				continue;
			}
			ISSettingsDatabase::Instance().SetValue(MetaField->Name, GetFieldValue(MetaField->Name));
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISSettingsDatabaseObjectForm::ResetSettings()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.ResetSettingDB")))
	{
		for (PMetaField *MetaField : GetMetaTable()->Fields) //Обходим все поля
		{
			if (MetaField->IsSystem) //Кроме системных
			{
				continue;
			}
			SetFieldValue(MetaField->Name, MetaField->DefaultValue);
		}

		if (Save())
		{
			ISMessageBox::ShowInformation(this, LANG("Message.Information.ResetSettingDB"));
		}
	}
}
//-----------------------------------------------------------------------------
