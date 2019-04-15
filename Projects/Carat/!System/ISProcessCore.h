#pragma once
//-----------------------------------------------------------------------------
#include <QProcess>
//-----------------------------------------------------------------------------
class ISProcessCore : public QProcess
{
	Q_OBJECT

signals:
	void Crash(const QString &CoreName);

public:
	ISProcessCore(const QString &name, const QString &local_name, const QString &core_file_name, QObject *parent);
	virtual ~ISProcessCore();

	QString GetName() const;
	QString GetLocalName() const;

	void SetRunning(bool running);
	bool GetRunning() const;

protected:
	void StateChanged(QProcess::ProcessState ProcessState);

private:
	QString Name;
	QString LocalName;
	bool IsRunning;
};
//-----------------------------------------------------------------------------
