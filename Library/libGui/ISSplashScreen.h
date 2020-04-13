#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSplashScreen : public QWidget
{
	Q_OBJECT

public:
	ISSplashScreen();
	virtual ~ISSplashScreen();
	
	void SetMessage(const QString &Message); //�������� ����� ���������

private:
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
