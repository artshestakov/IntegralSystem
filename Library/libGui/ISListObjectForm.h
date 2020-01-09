#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISListObjectForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISListObjectForm(const QString &TableName, int ParentObjectID, QWidget *parent = 0);
	virtual ~ISListObjectForm();
};
//-----------------------------------------------------------------------------
