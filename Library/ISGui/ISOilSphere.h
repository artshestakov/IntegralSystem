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

	//����� ������� ����������
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

	//����� ������� ������� ����������
	class ImplementationDetailObjectForm : public ISObjectFormBase
	{
		Q_OBJECT

	public:
		Q_INVOKABLE ImplementationDetailObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
		virtual ~ImplementationDetailObjectForm();

	private:
		void CalculateLoad(); //������ ��������
		void CalculateUnload(); //������ ��������
		void CalculateWeightDifference(); //������ ������� �����

	private:
		ISFieldEditBase *EditLoadContainer; //���� (����)
		ISFieldEditBase *EditLoadWeightGross; //��� (������)
		ISFieldEditBase *EditLoadWeightNet; //��� (�����)
		ISFieldEditBase *EditLoadPriceUnit; //���� �� ��./�
		ISFieldEditBase *EditLoadCost; //��������� (��������)

		ISFieldEditBase *EditUnloadContainer; //���� (����)
		ISFieldEditBase *EditUnloadWeightGross; //��� (������)
		ISFieldEditBase *EditUnloadWeightNet; //��� (�����)
		ISFieldEditBase *EditUnloadPriceUnit; //���� �� ��./�
		ISFieldEditBase *EditUnloadCost; //��������� (��������)

		ISFieldEditBase *EditWeightDifference; //������� �����
	};

	//����� ������� ��������� ���
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
