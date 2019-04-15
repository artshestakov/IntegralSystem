#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISContextMenuBase.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISContextMenuDateTime : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuDateTime(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty);
	virtual ~ISContextMenuDateTime();
};
//-----------------------------------------------------------------------------
