#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISObjectInterface : public QObject
{
	Q_OBJECT

public:
	ISObjectInterface(QObject *parent);
	virtual ~ISObjectInterface();

	virtual void RegisterMetaTypes() const = 0; //Регистрация мета-типов
	virtual void BeforeShowMainWindow() const = 0; //Событие перед открытие главного окна приложения
	virtual void InitializePlugin() const = 0; //Инициализация конфигурации
	QIcon GetIcon(const QString &IconName) const; //Получить икноку из ресурсов конфигурации
};
//-----------------------------------------------------------------------------
