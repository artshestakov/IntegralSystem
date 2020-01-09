#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUpdate : public QObject
{
	Q_OBJECT

signals:
	void Message(const QString &Message);
	void ProgressMaximum(int Maximum);
	void ProgressValue(int Value);

public:
	ISUpdate(const ISUpdate &) = delete;
	ISUpdate(ISUpdate &&) = delete;
	ISUpdate &operator=(const ISUpdate &) = delete;
	ISUpdate &operator=(ISUpdate &&) = delete;
	~ISUpdate();

	static ISUpdate& GetInstance();

	bool GetLoading() const;
	void SetStop(bool stop);

	bool CheckUpdate(int &FileID, QString &FileName, QString &Version);
	bool LoadUpdate(int FileID, const QString &FileName); //Загрузка обновления
	bool StartInstallUpdate(const QString &FileName); //Запуск установки обновления

private:
	ISUpdate();

	bool Loading;
	bool Stop;
	int Value;
};
//-----------------------------------------------------------------------------
