#pragma once
#ifndef _ISOILSPHERE_H_INCLUDED
#define _ISOILSPHERE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "ISObjectFormBase.h"
#include "ISFieldEdits.h"
#include "ISDaDataService.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
namespace ISOilSphere
{
	class Object : public ISObjectInterface
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Object();
		virtual ~Object();

		void RegisterMetaTypes() const override;
		void BeforeShowMainWindow() const override;
		void InitializePlugin() const override;
	};

	//Форма объекта контрагентов
	class CounterpartyObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE CounterpartyObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~CounterpartyObjectForm();

	private:
		void SearchFromINN();
		void SearchFinished(const ISDaDataOrganization &OrganizationStruct);

	private:
		ISINNEdit *INNEdit;
		ISDaDataService *DaDataService;
	};

	//Форма объекта реализации
	class ImplementationObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ImplementationObjectForm();

	private:
		void Saved(int ObjectID);
		void Updated();

	private:
		ISListBaseForm *ImplementationDetailListForm;
	};

	//Форма объекта деталей реализации
	class ImplementationDetailObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationDetailObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ImplementationDetailObjectForm();

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

	//Форма объекта ведомости АЗС
	class GasStationStatementObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE GasStationStatementObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~GasStationStatementObjectForm();

	protected:
		void SavedObject(int ObjectID);
	};
}
//-----------------------------------------------------------------------------
#endif
