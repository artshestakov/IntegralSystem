#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryPool : public QObject
{
	Q_OBJECT

public:
	static ISQueryPool& Instance();

	QString GetErrorString() const;
	bool Initialize();

	void AddQuery(const QString &SqlText); //�������� ������ � �������
	void AddQuery(const QString &SqlText, const QVariantMap &Parameters); //�������� ������ � �������

private:
	void StartExecuting();
	void ExecuteQuery();

private:
	ISQueryPool();
	~ISQueryPool();
	ISQueryPool(ISQueryPool const &) {};
	ISQueryPool& operator=(ISQueryPool const&) { return *this; };

private:
	QString ErrorString;
	QFutureWatcher<void> *FutureWatcher;
	QQueue<ISQueryPoolObject> Queue;
};
//-----------------------------------------------------------------------------
