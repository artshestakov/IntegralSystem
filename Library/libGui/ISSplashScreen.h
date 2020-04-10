#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSplashScreen : public QSplashScreen
{
	Q_OBJECT

public:
	ISSplashScreen();
	virtual ~ISSplashScreen();
	
	void SetMessage(const QString &Message); //�������� ����� ���������
	void ResetPixmap(); //�������� �����������
	void DefaultPixmap(); //���������� ���������� �����������

protected:
	void mousePressEvent(QMouseEvent *e);
};
//-----------------------------------------------------------------------------
