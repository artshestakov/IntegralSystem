#pragma once
#ifndef _ISOILSPHEREIMPLEMENTATIONOBJECTFORM_H_INCLUDED
#define _ISOILSPHEREIMPLEMENTATIONOBJECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
#include "ISFieldEdits.h"
#include "ISDaDataService.h"
//-----------------------------------------------------------------------------
class ISOilSphereImplementationObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOilSphereImplementationObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOilSphereImplementationObjectForm();
};
//-----------------------------------------------------------------------------
#endif
