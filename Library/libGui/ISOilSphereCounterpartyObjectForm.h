#pragma once
#ifndef _ISOILSPHERECOUNTERPARTYOBJECTFORM_H_INCLUDED
#define _ISOILSPHERECOUNTERPARTYOBJECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
#include "ISINNEdit.h"
#include "ISDaDataService.h"
//-----------------------------------------------------------------------------
class ISOilSphereCounterpartyObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOilSphereCounterpartyObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOilSphereCounterpartyObjectForm();

private:
	void SearchFromINN();
	void SearchFinished(const ISDaDataOrganization &OrganizationStruct);

private:
	ISINNEdit *INNEdit;
	ISDaDataService *DaDataService;
};
//-----------------------------------------------------------------------------
#endif
