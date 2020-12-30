#pragma once
#ifndef _ISDEVICECONNECTDIALOG_H_INCLUDED
#define _ISDEVICECONNECTDIALOG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDeviceConnectDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISDeviceConnectDialog();
	virtual ~ISDeviceConnectDialog();

	ISDeviceInfo& GetConnectedDevice();

private:
	void Timeout();

private:
	QTimer *Timer;
	std::vector<ISDeviceInfo> LastVector;
	ISDeviceInfo ConnectedDevice;
};
//-----------------------------------------------------------------------------
#endif
