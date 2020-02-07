#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISLicense
{
public:
	ISLicense(const ISLicense &) = delete;
	ISLicense(ISLicense &&) = delete;
	ISLicense &operator=(const ISLicense &) = delete;
	ISLicense &operator=(ISLicense &&) = delete;
	~ISLicense();

	static ISLicense& GetInstance();
	QString GetName() const; //Получить наименование конфигурации
	QString GetLocalName() const; //Получить локальное наименование кофигурации
	QString GetClassName() const; //Получить наименование класса ядра
	QString GetDesktopForm() const; //Получить наименование класса рабочего стола
	QString GetIncomingCallForm() const; //Получить наименование класса карточки входящего звонка
	QString GetErrorString() const; //Получить текст ошибки

	bool CheckExistLicenseTable() const; //Проверит наличие сущности лицензии
	bool CheckExistLicense() const; //Проверить наличие лицензии
	bool Initialize(); //Инициализация

protected:
	void ClearItems();
	void Delete(); //Удаление лицензии

private:
	ISLicense();

	QString ErrorString;
	QVector<ISLicenseItem*> VectorTemp;
	
	bool Initialized;
	QString ConfName;
	QString ConfLocalName;
	QString ConfClassName;
	QString ConfDesktopForm;
	QString ConfIncomingCallForm;
};
//-----------------------------------------------------------------------------
