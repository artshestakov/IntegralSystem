#pragma once
#ifndef _ISOILSPHERE_H_INCLUDED
#define _ISOILSPHERE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
#include "ISObjectFormBase.h"
#include "ISFieldEdits.h"
#include "ISListBaseForm.h"
#include "ISListViewForm.h"
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

    //Форма списка контрагентов
    class CounterpartySubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE CounterpartySubSystem(QWidget *parent = 0);
        virtual ~CounterpartySubSystem();

    private:
        void ShowDebt();
    };

    //Форма долгов контрагента
    class CounterpartyDebtForm : public ISInterfaceForm
    {
        Q_OBJECT

    public:
        CounterpartyDebtForm(int counterparty_id, const QString &counterparty_name, QWidget *parent = 0);
        virtual ~CounterpartyDebtForm();

    protected:
        void EscapeClicked() override;

    private:
        void UpdatedLists(); //Событие обновления списков
        void ShowImplementation();
        void ShowLoadUnload();

    private:
        int CounterpartyID;

    private:
        QLabel *LabelTotal;
        ISListBaseForm *EntrollmentListForm;
        ISListViewForm *MoveWagonViewForm;
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

    private:
        ISINNEdit *INNEdit;
    };

    //Форма объекта загрузки реализации
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

    //Форма списка ведомости АЗС
    class GasStationStatementSubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE GasStationStatementSubSystem(QWidget *parent = 0);
        virtual ~GasStationStatementSubSystem();

    private:
        void StockChanged(const QVariant &Value);

    private:
        ISComboEdit *EditStock;
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
        void FillInBased(); //Заполнить на основании
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

    //Форма объекта дебета 1
    class Debet1ObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet1ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~Debet1ObjectForm();

    private:
        void CalculateTotal(); //Расчёт "Итого"
        void CalculateRemainder(); //Расчёт "Остаток долга"
    };

    //Форма объекта дебета 2
    class Debet2ObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet2ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~Debet2ObjectForm();

    private:
        void CalculateTotal(); //Расчёт "Итого"
        void CalculateRemainder(); //Расчёт "Остаток долга"
    };

    //Форма объекта дебета 3
    class Debet3ObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet3ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~Debet3ObjectForm();

    private:
        void CalculateTotal(); //Расчёт "Итого"
        void CalculateRemainder(); //Расчёт "Остаток долга"
    };

    //Форма объекта дебета 4
    class Debet4ObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet4ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~Debet4ObjectForm();

    private:
        void CalculateTotal(); //Расчёт "Итого"
        void CalculateRemainder(); //Расчёт "Остаток долга"
    };

    //Форма объекта дебета 5
    class Debet5ObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet5ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~Debet5ObjectForm();

    private:
        void CalculateTotal(); //Расчёт "Итого"
        void CalculateRemainder(); //Расчёт "Остаток долга"
    };

    //Форма списка дебета 1
    class Debet1SubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet1SubSystem(QWidget *parent = 0);
        virtual ~Debet1SubSystem();

    protected:
        bool Update() override;

    private:
        QLabel *Label;
    };

    //Форма списка дебета 2
    class Debet2SubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet2SubSystem(QWidget *parent = 0);
        virtual ~Debet2SubSystem();

    protected:
        bool Update() override;

    private:
        QLabel *Label;
    };

    //Форма списка дебета 3
    class Debet3SubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet3SubSystem(QWidget *parent = 0);
        virtual ~Debet3SubSystem();

    protected:
        bool Update() override;

    private:
        QLabel *Label;
    };

    //Форма списка дебета 4
    class Debet4SubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet4SubSystem(QWidget *parent = 0);
        virtual ~Debet4SubSystem();

    protected:
        bool Update() override;

    private:
        QLabel *Label;
    };

    //Форма списка дебета 5
    class Debet5SubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE Debet5SubSystem(QWidget *parent = 0);
        virtual ~Debet5SubSystem();

    protected:
        bool Update() override;

    private:
        QLabel *Label;
    };

    //Форма списка расходов водителей
    class DriverCostSubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE DriverCostSubSystem(QWidget *parent = 0);
        virtual ~DriverCostSubSystem();

    private:
        void CreateOnBased(); //Создать на основании
    };

    //Форма постуления на склад ЖД
    class StockAdmissionTrain : public ISInterfaceMetaForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE StockAdmissionTrain(QWidget *parent = 0);
        virtual ~StockAdmissionTrain();

        void LoadData() override;

    private:
        ISListViewForm *ListViewForm;
    };

    //Форма поступления на склад с реализации
    class StockAdmissionImplemetation : public ISInterfaceMetaForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE StockAdmissionImplemetation(QWidget *parent = 0);
        virtual ~StockAdmissionImplemetation();

        void LoadData() override;

    private:
        ISListViewForm *ListViewForm;
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
        ISListViewForm *ListViewForm;
    };

    //Форма объекта приходов
    class ComingObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ComingObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~ComingObjectForm();

        bool Save() override;
    };

    //Форма объекта распределения
    class DistributionObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE DistributionObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~DistributionObjectForm();

        bool Save() override;
    };

    //Форма расходов пользователей
    class ConsumptionAllSubSystem : public ISInterfaceMetaForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ConsumptionAllSubSystem(QWidget *parent = 0);
        virtual ~ConsumptionAllSubSystem();

        void LoadData() override;

    private:
        void BalanceClicked();

    private:
        QVariantList UserList;

    private:
        QLabel *LabelTitle;
        QGroupBox *GroupBoxUsers;
        ISListWidget *ListConsumption;
        ISListWidget *ListReturn;
    };

    //Форма списка моих расходов
    class ConsumptionMySubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ConsumptionMySubSystem(QWidget *parent = 0);
        virtual ~ConsumptionMySubSystem();

        bool Update() override;
    };

    //Форма объекта расходов
    class ConsumptionObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ConsumptionObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~ConsumptionObjectForm();

        bool Save() override;
    };

    //Форма списка моих возвратов
    class ReturnMySubSystem : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ReturnMySubSystem(QWidget *parent = 0);
        virtual ~ReturnMySubSystem();

        bool Update() override;
    };

    //Форма объекта возврата
    class ReturnObjectForm : public ISObjectFormBase
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ReturnObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
        virtual ~ReturnObjectForm();

        bool Save() override;
    };

    //Форма списка банка
    class BankListForm : public ISListBaseForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE BankListForm(QWidget *parent = 0);
        virtual ~BankListForm();

    private:
        void Load();
    };

    class ComingSubSystem : public ISInterfaceMetaForm
    {
        Q_OBJECT

    public:
        Q_INVOKABLE ComingSubSystem(QWidget *parent = 0);
        virtual ~ComingSubSystem();

        void LoadData() override;

    private:
        ISListViewForm *EntrollmentListForm;
        ISListBaseForm *ComingListForm;
    };
}
//-----------------------------------------------------------------------------
#endif
