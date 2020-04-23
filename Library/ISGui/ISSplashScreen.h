#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISSplashScreen : public QWidget
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
