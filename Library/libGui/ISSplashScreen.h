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
	
	void SetMessage(const QString &Message); //Изменить текст сообщения
	void ResetPixmap(); //Сбросить изображение
	void DefaultPixmap(); //Установить станартное изображение

protected:
	void mousePressEvent(QMouseEvent *e);
};
//-----------------------------------------------------------------------------
