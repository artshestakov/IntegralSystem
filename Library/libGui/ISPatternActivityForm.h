#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISUuid.h"
#include "ISPatternWidget.h"
//-----------------------------------------------------------------------------
class ISPatternActivityForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void PatternData(const ISUuid &UID, const QString &IconName, const QString &Pattern, const QString &BackgroundColor);

public:
	ISPatternActivityForm(QWidget *parent = 0);
	virtual ~ISPatternActivityForm();

protected:
	void CreateMainPanel();
	void CreateSignPanel();

	void Timeout();
	void CheckStatus();
	void SetPatternData(const ISUuid &UID, const QString &IconName, const QString &Pattern, const QString &BackgroundColor);

private:
	QVBoxLayout *LayoutCentral;

	QTimer *Timer;
	QFutureWatcher<void> *FutureWatcher;
	QSqlDatabase Database;
	QMap<ISUuid, ISPatternWidget*> Map;
};
//-----------------------------------------------------------------------------
