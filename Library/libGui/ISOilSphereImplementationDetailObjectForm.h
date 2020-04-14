#pragma once
#ifndef _ISOILSPHEREIMPLEMENTATIONDETAILOBJECTFORM_H_INCLUDED
#define _ISOILSPHEREIMPLEMENTATIONDETAILOBJECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISOilSphereImplementationDetailObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOilSphereImplementationDetailObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOilSphereImplementationDetailObjectForm();

private:
	void CalculateLoad(); //Расчёт загрузки
	void CalculateUnload(); //Расчёт выгрузки
	void CalculateWeightDifference(); //Расчёт разницы весов

private:
	ISFieldEditBase *EditLoadContainer; //Тара (весы)
	ISFieldEditBase *EditLoadWeightGross; //Вес (брутто)
	ISFieldEditBase *EditLoadWeightNet; //Вес (нетто)
	ISFieldEditBase *EditLoadPriceUnit; //Цена за кг./л
	ISFieldEditBase *EditLoadCost; //Стоимость (загрузка)

	ISFieldEditBase *EditUnloadContainer; //Тара (весы)
	ISFieldEditBase *EditUnloadWeightGross; //Вес (брутто)
	ISFieldEditBase *EditUnloadWeightNet; //Вес (нетто)
	ISFieldEditBase *EditUnloadPriceUnit; //Цена за кг./л
	ISFieldEditBase *EditUnloadCost; //Стоимость (загрузка)

	ISFieldEditBase *EditWeightDifference; //Разница весов
};
//-----------------------------------------------------------------------------
#endif
