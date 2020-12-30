#include "ISDeviceConnectDialog.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtons.h"
#include "ISGui.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISDeviceConnectDialog::ISDeviceConnectDialog() : ISInterfaceDialogForm()
{
	resize(440, 220);
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	setWindowTitle(LANG("ISDeviceConnectDialog.Title"));
	setWindowIcon(BUFFER_ICONS("USBDevice"));

	QLabel *LabelTitle = new QLabel(LANG("ISDeviceConnectDialog.LabelInfo"), this);
	LabelTitle->setFont(ISDefines::Gui::FONT_TAHOMA_11);
	LabelTitle->setWordWrap(true);
	GetMainLayout()->addWidget(LabelTitle);

	GetMainLayout()->addStretch();

	QProgressBar *ProgressBar = new QProgressBar(this);
	ProgressBar->setRange(0, 0);
	ProgressBar->setTextVisible(false);
	GetMainLayout()->addWidget(ProgressBar);

	ISPushButton *ButtonClose = new ISPushButton(LANG("ISDeviceConnectDialog.Cancel"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISDeviceConnectDialog::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);

	Timer = new QTimer(this);
	Timer->setInterval(1000);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISDeviceConnectDialog::Timeout);
	Timer->start();
}
//-----------------------------------------------------------------------------
ISDeviceConnectDialog::~ISDeviceConnectDialog()
{
	
}
//-----------------------------------------------------------------------------
ISDeviceInfo& ISDeviceConnectDialog::GetConnectedDevice()
{
	return ConnectedDevice;
}
//-----------------------------------------------------------------------------
void ISDeviceConnectDialog::Timeout()
{
	//�������� ������ USB-���������
	std::vector<ISDeviceInfo> Vector;
	QString ErrorString;
	if (!ISGui::GetUSBDevice(Vector, ErrorString))
	{
		ISMessageBox::ShowCritical(this, ErrorString);
		close();
		return;
	}

	//���������, �� ��������� �� ����������
	if (Vector.size() > LastVector.size() && !LastVector.empty())
	{
		//���������� ��������� - ������� �� ������ ������� ���������� � ����������� ������
		for (size_t i = 0; i < LastVector.size(); ++i)
		{
			Vector.erase(std::remove_if(Vector.begin(), Vector.end(), [&](ISDeviceInfo const & DeviceInfo)
			{
				return DeviceInfo.VendorID == LastVector[i].VendorID &&
					DeviceInfo.ProductID == LastVector[i].ProductID &&
					DeviceInfo.SerialNumber == LastVector[i].SerialNumber;
			}), Vector.end());
		}

		//���� ����� �������� �������� ������ ������ ���������� - ���������� ����� ���������
		//���������� �� ���� � ���� ������
		if (Vector.size() > 1)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ConnectedMoreOneDevice"));
			Timer->start();
			return;
		}
		ConnectedDevice = Vector.front();
		SetResult(true);
		close();
	}
	else //���������� �� ��������� - ���������� �����
	{
		LastVector = Vector;
		Timer->start();
	}
}
//-----------------------------------------------------------------------------
