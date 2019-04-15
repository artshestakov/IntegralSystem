#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAsteriskCallsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAsteriskCallsListForm(QWidget *parent = 0);
	virtual ~ISAsteriskCallsListForm();

protected:
	void CreateDetailsPanel();
	void CreateStatusDescription();

	void DetailsClicked();
	void SaveRecord();
	void DoubleClickedTable(const QModelIndex &ModelIndex) override;
	void Back();

	void FillLabels();
	void PlayRecord();
	void SaveToStorage();
	void Play(const QString &FilePath);
	void StatusClicked();

	void DefaultFilter(); //Установить стандартный фильтр
	void TransferFilter(const QString &UniqueID); //Установить фильтр для переведенных звонков
	ISUuid GetStatusCall(); //Получить статус выделенного звонка

private:
	QAction *ActionDetails;
	QAction *ActionSave;
	QAction *ActionPlay;
	QAction *ActionStorage;
	QWidget *WidgetCallInfo;
	ISPushButton *ButtonBack;
	int ObjectID;

	QLabel *LabelDateTime;
	QLabel *LabelDirection;
	QLabel *LabelSubscriber;
	QLabel *LabelNumber;
	QLabel *LabelDuration;
	QLabel *LabelStatus;
};
//-----------------------------------------------------------------------------
