#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCurrentAffairsForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCurrentAffairsForm(QWidget *parent = 0);
	virtual ~ISCurrentAffairsForm();

protected:
	void LoadDataAfterEvent() override;
	void DoubleClickedTable(const QModelIndex &ModelIndex);
};
//-----------------------------------------------------------------------------
