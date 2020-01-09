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

	void RegisterForm(QWidget *ObjectForm); //���������������� ����� �������
	void UnregisterForm(const QString &FormUID); //�������� ����������� ����� �������
	bool CheckExistForms(); //�������� ������������ ����

private:
	ISCreatedObjectsEntity();

	QMap<ISUuid, QWidget*> ObjectForms;
};
//-----------------------------------------------------------------------------
