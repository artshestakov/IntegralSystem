#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class ISTaskImportantEdit : public ISCheckEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskImportantEdit(QWidget *parent);
	virtual ~ISTaskImportantEdit();

protected:
	void ImportantChanged(const QVariant &value);
};
//-----------------------------------------------------------------------------
