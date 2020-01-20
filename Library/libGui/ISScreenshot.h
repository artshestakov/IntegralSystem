#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISScreenshot : public QObject
{
	Q_OBJECT

public:
	ISScreenshot(const ISScreenshot &) = delete;
	ISScreenshot(ISScreenshot &&) = delete;
	ISScreenshot &operator=(const ISScreenshot &) = delete;
	ISScreenshot &operator=(ISScreenshot &&) = delete;
	~ISScreenshot();

	static ISScreenshot& GetInstance();

	void Create(const QVariantMap &VariantMap);

protected:
	void Creating(int Owner);

private:
	ISScreenshot();
	QFutureWatcher<void> *FutureWatcher;
	QSqlDatabase Database;
};
//-----------------------------------------------------------------------------
