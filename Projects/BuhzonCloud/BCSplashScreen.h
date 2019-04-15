#pragma once
//-----------------------------------------------------------------------------
#include <QSplashScreen>
//-----------------------------------------------------------------------------
class BCSplashScreen : public QSplashScreen
{
	Q_OBJECT

public:
	BCSplashScreen(QWidget *parent = 0);
	virtual ~BCSplashScreen();

protected:
	void mousePressEvent(QMouseEvent *e);
};
//-----------------------------------------------------------------------------
