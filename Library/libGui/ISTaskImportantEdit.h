#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskImportantEdit : public ISCheckEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskImportantEdit(QWidget *parent);
	virtual ~ISTaskImportantEdit();

protected:
	void ImportantChanged(const QVariant &value);
};
//-----------------------------------------------------------------------------
