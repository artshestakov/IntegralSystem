#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISReconnectForm : public ISInterfaceDialogForm
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
