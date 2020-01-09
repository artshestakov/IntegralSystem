#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCreatedObjectsEntity : public QObject
{
	Q_OBJECT

signals:
	void Existed();

public:
	ISCreatedObjectsEntity(const ISCreatedObjectsEntity &) = delete;
	ISCreatedObjectsEntity(ISCreatedObjectsEntity &&) = delete;
	ISCreatedObjectsEntity &operator=(const ISCreatedObjectsEntity &) = delete;
	ISCreatedObjectsEntity &operator=(ISCreatedObjectsEntity &&) = delete;
	~ISCreatedObjectsEntity();

	static ISCreatedObjectsEntity& GetInstance();

	void RegisterForm(QWidget *ObjectForm); //Зарегистрировать форму объекта
	void UnregisterForm(const QString &FormUID); //Отменить регистрацию формы объекта
	bool CheckExistForms(); //Проверка существующих форм

private:
	ISCreatedObjectsEntity();

	QMap<ISUuid, QWidget*> ObjectForms;
};
//-----------------------------------------------------------------------------
