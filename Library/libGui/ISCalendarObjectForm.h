#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCalendarObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCalendarObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISCalendarObjectForm();

	void SetDate(const QDate &Date);
	bool Save() override;

protected:
	void AfterShowEvent() override;
	void EscapeClicked() override;
	void OpenCard();
};
//-----------------------------------------------------------------------------
