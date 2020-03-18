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

signals:
	void SetProgressMaximum(int);
	void SetProgressValue(int);
	void SetLabelSearch(const QString &);

public:
	Q_INVOKABLE ISFullTextSearchForm(QWidget *parent = 0);
	virtual ~ISFullTextSearchForm();

	void LoadData() override;

private:
	void SetSearchInProgress(bool InProgress);
	void Search();
	void Execute(const QVariant &SearchValue);

	QString CreateQuery(PMetaTable *MetaTable) const;
	void ClickedRecord();
	void Stop();

private:
	QEventLoop *EventLoop;
	QFutureWatcher<void> *FutureWatcher;
	QWidgetList WidgetList;
	std::map<QString, ISVectorInt> MapResult;
	std::mutex Mutex;
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
