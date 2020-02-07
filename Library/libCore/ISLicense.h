#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISLicense
{
public:
	ISLicense(const ISLicense &) = delete;
	ISLicense(ISLicense &&) = delete;
	ISLicense &operator=(const ISLicense &) = delete;
	ISLicense &operator=(ISLicense &&) = delete;
	~ISLicense();

	static ISLicense& GetInstance();
	QString GetName() const; //�������� ������������ ������������
	QString GetLocalName() const; //�������� ��������� ������������ �����������
	QString GetClassName() const; //�������� ������������ ������ ����
	QString GetDesktopForm() const; //�������� ������������ ������ �������� �����
	QString GetIncomingCallForm() const; //�������� ������������ ������ �������� ��������� ������
	QString GetErrorString() const; //�������� ����� ������

	bool CheckExistLicenseTable() const; //�������� ������� �������� ��������
	bool CheckExistLicense() const; //��������� ������� ��������
	bool Initialize(); //�������������

protected:
	void ClearItems();
	void Delete(); //�������� ��������

private:
	ISLicense();

	QString ErrorString;
	QVector<ISLicenseItem*> VectorTemp;
	
	bool Initialized;
	QString ConfName;
	QString ConfLocalName;
	QString ConfClassName;
	QString ConfDesktopForm;
	QString ConfIncomingCallForm;
};
//-----------------------------------------------------------------------------
