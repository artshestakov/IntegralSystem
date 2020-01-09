#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISContextMenuBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISContextMenuDouble : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuDouble(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, double Value, double Minimum, double Maximum);
	virtual ~ISContextMenuDouble();
};
//-----------------------------------------------------------------------------
