#pragma once
#ifndef _ISTASKOBJECTFORM_H_INCLUDED
#define _ISTASKOBJECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISTaskObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISTaskObjectForm();
};
//-----------------------------------------------------------------------------
#endif
