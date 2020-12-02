#pragma once
#ifndef _ISRECONNECTFORM_H_INCLUDED
#define _ISRECONNECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISReconnectForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISReconnectForm();
	~ISReconnectForm();

private:
	void Timeout();

private:
	QLabel *LabelAttempts;
	QTimer *Timer;

	unsigned int Attempts;
};
//-----------------------------------------------------------------------------
#endif
