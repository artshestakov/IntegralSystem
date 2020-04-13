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
	
	void SetMessage(const QString &Message); //Изменить текст сообщения

private:
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
