#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISReconnectForm : public ISInterfaceDialogForm
{
	Q_OBJECT

signals:
	void ConnectInstalled();

public:
	ISReconnectForm(const ISReconnectForm &) = delete;
	ISReconnectForm(ISReconnectForm &&) = delete;
	ISReconnectForm &operator=(const ISReconnectForm &) = delete;
	ISReconnectForm &operator=(ISReconnectForm &&) = delete;
	~ISReconnectForm();

	static ISReconnectForm& GetInstance();
	
	void StartReconnect(bool &Connect);

protected:
	bool Connect();
	void Restart();
	void Exit();
	void Hide();
	void AddLabel(const QString &Text);
	void TimerTick();
	void EscapeClicked() override;

private:
	ISReconnectForm();

	QVBoxLayout *Layout;
	QProgressBar *ProgressBar;
	QLabel *LabelCurrentStatus;
	QTimer *Timer = nullptr;

	int CurrentAttemp;
	bool Connected;
};
//-----------------------------------------------------------------------------
