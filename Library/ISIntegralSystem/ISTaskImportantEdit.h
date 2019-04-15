#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskImportantEdit : public ISCheckEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskImportantEdit(QWidget *parent);
	virtual ~ISTaskImportantEdit();

protected:
	void ImportantChanged(const QVariant &value);
};
//-----------------------------------------------------------------------------
