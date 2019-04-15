#pragma once
//-----------------------------------------------------------------------------
#include <QObject>
//#include <QNetworkAccessManager>
//-----------------------------------------------------------------------------
class BCContentManager : public QObject
{
	Q_OBJECT

public:
	BCContentManager(const BCContentManager &) = delete;
	BCContentManager(BCContentManager &&) = delete;
	BCContentManager &operator=(const BCContentManager &) = delete;
	BCContentManager &operator=(BCContentManager &&) = delete;
	~BCContentManager();

	static BCContentManager& GetInstance();
	QByteArray GetFile();
	QString GetExtension() const;

protected:
	bool CheckExistLocal(const QString &ParameterName);
	int GetRandom(int Max);

private:
	BCContentManager();

	QNetworkAccessManager *NetworkAccessManager;
	QString FileIndex;
	QStringList FileList;
	QString LastExtension;
};
//-----------------------------------------------------------------------------
