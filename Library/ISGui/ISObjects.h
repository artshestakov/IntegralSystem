#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISObjects
{
public:
	static ISObjects& Instance();

public:
	QString GetErrorString() const;
	void Initialize(const QString &ConfigurationName);
	ISObjectInterface* GetInterface(); //�������� ��������� �� ���� ������������

private:
	ISObjects();
	~ISObjects();
	ISObjects(ISObjects const &) {};
	ISObjects& operator=(ISObjects const&) { return *this; };

private:
	QString ErrorString;
	ISObjectInterface *ObjectInterface;
};
//-----------------------------------------------------------------------------
