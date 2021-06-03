#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISObjectInterface : public QObject
{
    Q_OBJECT

public:
    ISObjectInterface();
    virtual ~ISObjectInterface();

    void SetConfigurationName(const QString &configuration_name);

    virtual void RegisterMetaTypes() const = 0; //Регистрация мета-типов
    virtual void BeforeShowMainWindow() const = 0; //Событие перед открытие главного окна приложения
    virtual void InitializePlugin() const = 0; //Инициализация конфигурации
    QIcon GetIcon(const QString &IconName) const; //Получить икноку из ресурсов конфигурации

private:
    QString ConfigurationName;
};
//-----------------------------------------------------------------------------
