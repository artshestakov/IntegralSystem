#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISVersion : public QObject
{
	Q_OBJECT

public:
	ISVersion(const ISVersion &) = delete;
	ISVersion(ISVersion &&) = delete;
	ISVersion &operator=(const ISVersion &) = delete;
	ISVersion &operator=(ISVersion &&) = delete;
	~ISVersion();

	static ISVersion& GetInstance();
	void Initialize();
	void SetVersion(const QString &version); //Изменить версию

	bool IsValid() const; //Валидность версии
	QString GetVersion() const; //Получить полную версию
	int GetMajor() const; //Получить номер мажорной версии
	int GetMinor() const; //Получить номер минорной версии
	int GetRevision() const; //Получить номер ревизии из репозитория
	int GetBuild() const; //Получить номер сборки
	QDate GetDate() const; //Получить дату сборки
	QTime GetTime() const; //Получить время сборки
	QString GetHash() const; //Получить хеш репозитория
	QString GetPlatform() const; //Получить платформу приложения

protected:
	QString GetFileData(const QString &FileName) const;

private:
	ISVersion();

	QString VersionString;
	int Major;
	int Minor;
	int Revision;
	int Build;
	QDate Date;
	QTime Time;
	QString Hash;
	QString Platform;
};
//-----------------------------------------------------------------------------
