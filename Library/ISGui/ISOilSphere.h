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
	class CounterpartyListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE CounterpartyListForm(QWidget *parent = 0);
		virtual ~CounterpartyListForm();

	private:
		void ShowDebt();
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
		void MoveWagonUpdated();
		void ShowImplementation();
		void ShowLoadUnload();

	private:
		double TotalLoad;
		double TotalUnload;
		double MoveWagonSum;

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
		void SearchFinished(const ISDaDataOrganization &OrganizationStruct);

	private:
		ISINNEdit *INNEdit;
		ISDaDataService *DaDataService;
	};

	//����� ������ ����������
	class ImplementationListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationListForm(QWidget *parent = 0);
		~ImplementationListForm();

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

	//����� ������ ������ 1
	class Debet1ListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet1ListForm(QWidget *parent = 0);
		virtual ~Debet1ListForm();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ ������ 2
	class Debet2ListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet2ListForm(QWidget *parent = 0);
		virtual ~Debet2ListForm();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ ������ 3
	class Debet3ListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE Debet3ListForm(QWidget *parent = 0);
		virtual ~Debet3ListForm();

	protected:
		void LoadDataAfterEvent() override;

	private:
		QLabel *Label;
	};

	//����� ������ �������� ���������
	class DriverCostListForm : public ISListBaseForm
	{
		Q_OBJECT

	public:
		Q_INVOKABLE DriverCostListForm(QWidget *parent = 0);
		virtual ~DriverCostListForm();

	private:
		void CreateOnBased(); //������� �� ���������
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
}
//-----------------------------------------------------------------------------
#endif
