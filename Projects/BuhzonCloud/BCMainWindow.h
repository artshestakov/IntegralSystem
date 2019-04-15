#pragma once
//-----------------------------------------------------------------------------
#include <QWidget>
//-----------------------------------------------------------------------------
class BCMainWindow : public QWidget
{
	Q_OBJECT

signals:
	void HideBanner();

public:
	BCMainWindow(QWidget *parent = 0);
	virtual ~BCMainWindow();

protected:
	void keyPressEvent(QKeyEvent *e);
	void showEvent(QShowEvent *e);

	void LoadContent();
	void Connect();
	void ReadyReadStandardOutput();
	void ReadyReadStandardError();
	void GenerateRdpFile(const QString &Password);
	void SetWaitCursor(bool Wait); //Изменить глобальный курсор для ожидания

private:
	QLineEdit *EditLogin;
	QLineEdit *EditPassword;
	QComboBox *ComboBox;
	QCheckBox *CheckRemember;
	QCheckBox *CheckHideContent;
	QPushButton *ButtonConnect;
	QPushButton *ButtonExit;
	QLabel *LabelContent;

	QProcess *Process;
	QSettings *Settings;
};
//-----------------------------------------------------------------------------
