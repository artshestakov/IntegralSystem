#pragma once
#ifndef _ISSETTINGSDATABASEOBJETCFORM_H_INCLUDED
#define _ISSETTINGSDATABASEOBJETCFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISSettingsDatabaseObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSettingsDatabaseObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISSettingsDatabaseObjectForm();

	bool Save() override;

private:
	void ResetSettings();
};
//-----------------------------------------------------------------------------
#endif
