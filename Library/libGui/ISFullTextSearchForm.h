#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISLineEdit.h"
#include "PMetaClass.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
class ISFullTextSearchForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFullTextSearchForm(QWidget *parent = 0);
	virtual ~ISFullTextSearchForm();

	void LoadData() override;

protected:
	void BeforeSearch();
	void Search();
	void AfterSearch();
	void Execute(const QString &QueryText, const QVariant &QueryValue);

	QString CreateQuery(PMetaClassTable *MetaTable) const;
	void ClickedRecord();
	void Stop();

private:
	QSqlDatabase Database;
	QEventLoop *EventLoop;
	QFutureWatcher<void> *FutureWatcher;
	QWidgetList WidgetList;
	QMap<QString, QVectorInt> Map;
	bool Stopped;

	ISLineEdit *LineEdit;
	ISPushButton *ButtonSearch;
	ISPushButton *ButtonStop;
	QProgressBar *ProgressBar;
	QLabel *LabelSearch;
	QLabel *LabelResult;
	QFrame *Frame;
	ISScrollArea *ScrollArea;
};
//-----------------------------------------------------------------------------
