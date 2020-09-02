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

	//Форма объекта загрузок реализации
	class ImplementationLoadObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationLoadObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ImplementationLoadObjectForm();

		bool Save() override;

	private:
		void Calculate();
	};

	//Форма объекта выгрузки реализации
	class ImplementationUnloadObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationUnloadObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ImplementationUnloadObjectForm();

		bool Save() override;

	private:
		void Calculate();
	};

	//Форма объекта ведомости АЗС
	class GasStationStatementObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE GasStationStatementObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~GasStationStatementObjectForm();

	protected:
		void AfterShowEvent() override;

	private:
		void CalculateBalanceEndChange(); //Расчёт поля "Остаток на конец смены"
		void CalculateCashboxBalancePayment(); //Расчёт поля "Остаток в кассе, расчёт"
		void CalculateCashboxBalanceActually(); //Расчёт поля "Остаток в кассе, фактический"
		void CalculateCashboxDiscrepancies(); //Расчёт поля "Расхождения по кассе"
		void CalculateCashboxKKMCash(); //Расчёт поля "Наличные по ККМ (касса)"
		void CalculateCashboxTotalPayment(); //Расчёт поля "Накопительный итог, расчёт"
		void CalculateCashboxTotalActually(); //Расчёт поля "Накопительный итог, фактический"
		void CaclulateCashboxDiscrepanciesTotals(); //Расчёт поля "Расхождения итогов"
		void CalculateCashboxKKMTotal(); //Расчёт поля "Накопительный итог по ККМ"

	private:
		//Данные за прошлую смену (для расчётов)
		double BeforeBalanceBeginChange;
		double BeforeCashboxTotalPayment;
		double BeforeCashboxTotalActually;
		double BeforeCashboxKKMTotal;
	};

	//Форма для подсистемы долгов
	class DebtSubSystemForm : public ISInterfaceMetaForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE DebtSubSystemForm(QWidget *parent = 0);
		virtual ~DebtSubSystemForm();

		void LoadData() override;

	private:
		void AddAccrued();
		void ShowImplementation();
		void ShowImplementationUnload();
		void ShowCounterparty();

	private:
		QWidget* CreateItemWidget(int ImplementationID, int ImplementationUnloadID, int CounterpartyID, const QString &CounterpartyName, double Balance, const QDate &DateLoad, const QString &ProductTypeName, double Cost, int Accrued);

	private:
		QTreeWidget *TreeWidget;
	};

	//Форма объекта дебета 1
	class DebetObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE DebetObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~DebetObjectForm();

	private:
		void CalculateTotal(); //Расчёт "Итого"
		void CalculateRemainder(); //Расчёт "Остаток долга"
	};

	//Форма списка дебета 1
	class DebetListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE DebetListForm(QWidget *parent = 0);
		virtual ~DebetListForm();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *LabelTotalSum; //Сумма итого
		QLabel *LabelRemainderSum; //Сумма остатка
	};

	//Форма объекта расходов СУГ
	class ConsumptionSUGObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ConsumptionSUGObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ConsumptionSUGObjectForm();

	private:
		void CalculateRemainder(); //Расчёт поля "Остаток"
	};

	//Форма пополнения склада
	class ArrivalStock : public ISInterfaceMetaForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ArrivalStock(QWidget *parent = 0);
		virtual ~ArrivalStock();

		void LoadData() override;

	private:
		ISBaseTableView *TableView;
	};

	//Форма списаний со склада
	class StockWriteOff : public ISInterfaceMetaForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE StockWriteOff(QWidget *parent = 0);
		virtual ~StockWriteOff();

		void LoadData() override;

	private:
		ISBaseTableView *TableView;
	};
}
//-----------------------------------------------------------------------------
#endif
