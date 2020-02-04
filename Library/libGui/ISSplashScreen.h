#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSplashScreen : public QSplashScreen
{
	Q_OBJECT

public:
	ISSplashScreen(const ISSplashScreen &) = delete;
	ISSplashScreen(ISSplashScreen &&) = delete;
	ISSplashScreen &operator=(const ISSplashScreen &) = delete;
	ISSplashScreen &operator=(ISSplashScreen &&) = delete;
	~ISSplashScreen();
	
	static ISSplashScreen& GetInstance();
	void SetMessage(const QString &Message); //�������� ����� ���������

	void ResetPixmap(); //�������� �����������
	void DefaultPixmap(); //���������� ���������� �����������

protected:
	void mousePressEvent(QMouseEvent *e);

private:
	ISSplashScreen(QWidget *parent = 0);

	QProgressBar *ProgressBar;
};
//-----------------------------------------------------------------------------
