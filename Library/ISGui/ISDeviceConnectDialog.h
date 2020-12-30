#pragma once
#ifndef _ISDEVICECONNECTDIALOG_H_INCLUDED
#define _ISDEVICECONNECTDIALOG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDeviceConnectDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISDeviceConnectDialog();
	virtual ~ISDeviceConnectDialog();

protected:
	void AfterShowEvent() override;
};
//-----------------------------------------------------------------------------
#endif
