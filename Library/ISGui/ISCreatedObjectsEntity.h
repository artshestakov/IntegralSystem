#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISCreatedObjectsEntity : public QObject
{
	Q_OBJECT

signals:
	void Existed();

public:
	static ISCreatedObjectsEntity& Instance();

	void RegisterForm(ISObjectFormBase *ObjectForm); //���������������� ����� �������
	void UnregisterForm(const QString &FormUID); //�������� ����������� ����� �������
	bool CheckExistForms(); //�������� ������������ ����

private:
	ISCreatedObjectsEntity();
	~ISCreatedObjectsEntity();
	ISCreatedObjectsEntity(ISCreatedObjectsEntity const &) {};
	ISCreatedObjectsEntity& operator=(ISCreatedObjectsEntity const&) { return *this; };
	
private:
	std::map<ISUuid, ISObjectFormBase*> ObjectForms;
};
//-----------------------------------------------------------------------------
