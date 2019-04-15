#pragma once
//-----------------------------------------------------------------------------
#include <ISProcess.h>
//-----------------------------------------------------------------------------
class DSWorker : public QObject
{
	Q_OBJECT

public:
	DSWorker(QObject *parent = 0);
	virtual ~DSWorker();

	void ClearBackupFolder();
	void Start();

protected:
	void ProcessMessage(const QString &Message);
	QStringList GetDumpArguments(const QString &FilePath) const; //Получить аргументы для резервной копии базы данных
	QStringList GetDumpAllArguments(const QString &FilePath) const; //Получить аргументы для ролей резервной копии базы данных

private:
	QEventLoop *EventLoop;
	ISProcess *Process;

	QString DatabaseName;
	QString FilePath;
};
//-----------------------------------------------------------------------------
