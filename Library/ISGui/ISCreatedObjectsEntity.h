#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCreatedObjectsEntity : public QObject
{
	Q_OBJECT

signals:
	void Existed();

public:
	static ISCreatedObjectsEntity& Instance();

	void RegisterForm(QWidget *ObjectForm); //Зарегистрировать форму объекта
	void UnregisterForm(const QString &FormUID); //Отменить регистрацию формы объекта
	bool CheckExistForms(); //Проверка существующих форм

private:
	ISCreatedObjectsEntity();
	~ISCreatedObjectsEntity();
	ISCreatedObjectsEntity(ISCreatedObjectsEntity const &) {};
	ISCreatedObjectsEntity& operator=(ISCreatedObjectsEntity const&) { return *this; };
	
private:
	std::map<ISUuid, QWidget*> ObjectForms;
};
//-----------------------------------------------------------------------------
