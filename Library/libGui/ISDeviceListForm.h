#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
#include "ISDoubleEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDeviceListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceListForm(QWidget *parent = 0);
	virtual ~ISDeviceListForm();

protected:
	void Issue();
	QString GetPhoneNumber();
	int GetTotalSum();
	void Update() override;
	void SelectedRowEvent(const QItemSelection &ItemSelected, const QItemSelection &ItemDeSelected) override;

private:
	ISDoubleEdit *DoubleEdit;
};
//-----------------------------------------------------------------------------
