#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISRangeEdits.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISAsteriskTotalForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAsteriskTotalForm(QWidget *parent = 0);
	virtual ~ISAsteriskTotalForm();

protected:
	void CreateTotal();
	void CreateUsers();

	void Update();
	void ReloadTotal();
	void ReloadUsers();

private:
	ISRangeDateTimeEdit *RangeDateTimeEdit;
	ISPushButton *ButtonRange;

	QLabel *LabelCountTotal; //Всего звонков
	QLabel *LabelCountIncoming; //Входящие
	QLabel *LabelCountOutcoming; //Исходящие

	QLabel *LabelDurationTotal; //Общая длительность
	QLabel *LabelDurationIncoming; //Длительность входящих
	QLabel *LabelDurationOutcoming; //Длительность исходящих

	QLabel *LabelAverageIncoming; //Средняя продолжительность исходящих
	QLabel *LabelAverageOutcoming; //Средняя продолжительность входящих

	QTreeWidget *TreeWidget;
};
//-----------------------------------------------------------------------------
