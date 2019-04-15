#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISObjects : public QObject
{
	Q_OBJECT

public:
	ISObjects(const ISObjects &) = delete;
	ISObjects(ISObjects &&) = delete;
	ISObjects &operator=(const ISObjects &) = delete;
	ISObjects &operator=(ISObjects &&) = delete;
	~ISObjects();

	static ISObjects& GetInstance();
	void Initialize(); //�������������
	ISObjectInterface* GetInterface(); //�������� ��������� �� ���� ������������

private:
	ISObjects();

	ISObjectInterface *ObjectInterface;
};
//-----------------------------------------------------------------------------
