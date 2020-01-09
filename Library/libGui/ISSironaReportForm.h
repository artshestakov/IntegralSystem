#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceMetaForm.h"
#include "ISRangeDateEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSironaReportForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaReportForm(QWidget *parent = 0);
	virtual ~ISSironaReportForm();

	void LoadData() override;

protected:
	void OutputList();
	double GetDoctorBonus(int DoctorID) const;

private:
	ISRangeDateEdit *RangeDateEdit;
	QTreeWidget *TreeWidget;
};
//-----------------------------------------------------------------------------
