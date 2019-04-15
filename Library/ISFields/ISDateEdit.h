#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISDateTimeEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISDateEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDateEdit(QWidget *parent);
	virtual ~ISDateEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

protected:
	void DateEnableChanged(int State) override;

	void BeforeYesterday(); //���������
	void Yesterday(); //�����
	void Today(); //�������
	void Tomorrow(); //������
	void AfterTomorrow(); //�����������

	void SetActionsIcon(const QDate &Date);

private:
	QAction *ActionBeforeYesterday;
	QAction *ActionYesterday;
	QAction *ActionToday;
	QAction *ActionTomorrow;
	QAction *ActionAfterTomorrow;
};
//-----------------------------------------------------------------------------
