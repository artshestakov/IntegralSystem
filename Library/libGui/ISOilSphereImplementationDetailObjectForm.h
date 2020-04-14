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
//-----------------------------------------------------------------------------
#endif
