#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISContextMenuBase.h"
//-----------------------------------------------------------------------------
//Контекстное меню для полей редактирования целых чисел (Преимущественно ISQSpinBox)
class LIBGUI_EXPORT ISContextMenuInteger : public ISContextMenuBase
{
	Q_OBJECT

signals:
	void StepUp();
	void StepDown();

public:
	ISContextMenuInteger(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int Value, int Minimum, int Maximum);
	virtual ~ISContextMenuInteger();
};
//-----------------------------------------------------------------------------
