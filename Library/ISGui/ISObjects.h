#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISObjectInterface.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISObjects
{
public:
	ISObjects(const ISObjects &) = delete;
	ISObjects(ISObjects &&) = delete;
	ISObjects &operator=(const ISObjects &) = delete;
	ISObjects &operator=(ISObjects &&) = delete;
	~ISObjects();

	static ISObjects& GetInstance();

	QString GetErrorString() const;
	bool IsInitialized() const;
	bool Initialize(); //�������������

	ISObjectInterface* GetInterface(); //�������� ��������� �� ���� ������������
	ISConfigurationItem GetInfo(); //�������� ���������� � ������������

private:
	ISObjects();

	QString ErrorString;
	QString ConfigurationName;
	ISObjectInterface *ObjectInterface;
	ISConfigurationItem Info;
};
//-----------------------------------------------------------------------------
