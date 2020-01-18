#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class PMetaClass : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString Name READ GetName WRITE SetName)
	Q_PROPERTY(QString UID READ GetUID WRITE SetUID)
	Q_PROPERTY(QString TypeObject READ GetTypeObject WRITE SetTypeObject)

public:
	PMetaClass(QObject *parent = 0);
	virtual ~PMetaClass();

	void SetName(const QString &name); //Изменить имя объекта
	QString GetName() const; //Получить имя объекта

	void SetUID(const QString &uid); //Изменить идентификатор объекта
	QString GetUID() const; //Получить идентификатор объекта

	void SetTypeObject(const QString &type_object); //Изменить имя типа объекта
	QString GetTypeObject() const; //Получить тип объекта

private:
	QString Name; //Имя
	QString UID; //Идентификатор
	QString TypeObject; //Тип
};
//-----------------------------------------------------------------------------
