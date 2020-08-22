#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISObjectInterface.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISObjects
{
public:
	static ISObjects& Instance();
	ISConfigurationItem Info; //���������� � ������������

public:
	QString GetErrorString() const;
	bool IsInitialized() const;
	bool Initialize(); //�������������
	ISObjectInterface* GetInterface(); //�������� ��������� �� ���� ������������

private:
	ISObjects();
	~ISObjects();
	ISObjects(ISObjects const &) {};
	ISObjects& operator=(ISObjects const&) { return *this; };

private:
	QString ErrorString;
	QString ConfigurationName;
	ISObjectInterface *ObjectInterface;
};
//-----------------------------------------------------------------------------
