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

	//����� ������ ������������
	class CounterpartySubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE CounterpartySubSystem(QWidget *parent = 0);
		virtual ~CounterpartySubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		void ShowDebt();

	private:
		QLabel *LabelTotal;
	};

	//����� ������ �����������
	class CounterpartyDebtForm : public ISInterfaceForm
	{
		Q_OBJECT

	public:
		CounterpartyDebtForm(int counterparty_id, const QString &counterparty_name, QWidget *parent = 0);
		virtual ~CounterpartyDebtForm();

	protected:
		void EscapeClicked() override;

	private:
		void UpdatedLists(); //������� ���������� �������
		void ShowImplementation();
		void ShowLoadUnload();

	private:
		int CounterpartyID;

	private:
		QLabel *LabelTotal;
		ISListBaseForm *EntrollmentListForm;
		ISListViewForm *MoveWagonViewForm;
	};

	//����� ������� ������������
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

	//����� ������ ����������
	class ImplementationSubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationSubSystem(QWidget *parent = 0);
		~ImplementationSubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *LabelTotal;
	};

	//����� ������� �������� ����������
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

	//����� ������� �������� ����������
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

	//����� ������ ��������� ���
	class GasStationStatementSubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE GasStationStatementSubSystem(QWidget *parent = 0);
		virtual ~GasStationStatementSubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		void StockChanged(const QVariant &Value);

	private:
		ISComboEdit *EditStock;
		QLabel *LabelTotal;
	};

	//����� ������� ��������� ���
	class GasStationStatementObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE GasStationStatementObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~GasStationStatementObjectForm();

	protected:
		void AfterShowEvent() override;

	private:
		void FillInBased(); //��������� �� ���������
		void CalculateBalanceEndChange(); //������ ���� "������� �� ����� �����"
		void CalculateCashboxBalancePayment(); //������ ���� "������� � �����, ������"
		void CalculateCashboxBalanceActually(); //������ ���� "������� � �����, �����������"
		void CalculateCashboxDiscrepancies(); //������ ���� "����������� �� �����"
		void CalculateCashboxKKMCash(); //������ ���� "�������� �� ��� (�����)"
		void CalculateCashboxTotalPayment(); //������ ���� "������������� ����, ������"
		void CalculateCashboxTotalActually(); //������ ���� "������������� ����, �����������"
		void CaclulateCashboxDiscrepanciesTotals(); //������ ���� "����������� ������"
		void CalculateCashboxKKMTotal(); //������ ���� "������������� ���� �� ���"

	private:
		//������ �� ������� ����� (��� ��������)
		double BeforeBalanceBeginChange;
		double BeforeCashboxTotalPayment;
		double BeforeCashboxTotalActually;
		double BeforeCashboxKKMTotal;
	};

	//����� ������� ������ 1
	class Debet1ObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet1ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~Debet1ObjectForm();

	private:
		void CalculateTotal(); //������ "�����"
		void CalculateRemainder(); //������ "������� �����"
	};

	//����� ������� ������ 2
	class Debet2ObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet2ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~Debet2ObjectForm();

	private:
		void CalculateTotal(); //������ "�����"
		void CalculateRemainder(); //������ "������� �����"
	};

	//����� ������� ������ 3
	class Debet3ObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet3ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~Debet3ObjectForm();

	private:
		void CalculateTotal(); //������ "�����"
		void CalculateRemainder(); //������ "������� �����"
	};

	//����� ������� ������ 4
	class Debet4ObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet4ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~Debet4ObjectForm();

	private:
		void CalculateTotal(); //������ "�����"
		void CalculateRemainder(); //������ "������� �����"
	};

	//����� ������� ������ 5
	class Debet5ObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet5ObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~Debet5ObjectForm();

	private:
		void CalculateTotal(); //������ "�����"
		void CalculateRemainder(); //������ "������� �����"
	};

	//����� ������ ������ 1
	class Debet1SubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet1SubSystem(QWidget *parent = 0);
		virtual ~Debet1SubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ ������ 2
	class Debet2SubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet2SubSystem(QWidget *parent = 0);
		virtual ~Debet2SubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ ������ 3
	class Debet3SubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet3SubSystem(QWidget *parent = 0);
		virtual ~Debet3SubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ ������ 4
	class Debet4SubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet4SubSystem(QWidget *parent = 0);
		virtual ~Debet4SubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ ������ 5
	class Debet5SubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet5SubSystem(QWidget *parent = 0);
		virtual ~Debet5SubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ �������� ���������
	class DriverCostSubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE DriverCostSubSystem(QWidget *parent = 0);
		virtual ~DriverCostSubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		void CreateOnBased(); //������� �� ���������

	private:
		QLabel *LabelTotal;
	};

	//����� ���������� �� ����� ��
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

	//����� ����������� �� ����� � ����������
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

	//����� �������� �� ������
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

	//����� ������ �������� �������
	class MoveWagonSubSystem : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE MoveWagonSubSystem(QWidget *parent = 0);
		virtual ~MoveWagonSubSystem();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *LabelTotal;
	};
}
//-----------------------------------------------------------------------------
#endif
