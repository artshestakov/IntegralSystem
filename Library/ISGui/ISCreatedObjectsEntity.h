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
	ISCreatedObjectsEntity(const ISCreatedObjectsEntity&) = delete;
	ISCreatedObjectsEntity(ISCreatedObjectsEntity&&) = delete;
	ISCreatedObjectsEntity& operator=(const ISCreatedObjectsEntity&) = delete;
	ISCreatedObjectsEntity& operator=(ISCreatedObjectsEntity&&) = delete;
	
private:
	std::map<ISUuid, ISObjectFormBase*> ObjectForms;
};
//-----------------------------------------------------------------------------
