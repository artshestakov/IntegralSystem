#include "StdAfx.h"
#include "BCSplashScreen.h"
//-----------------------------------------------------------------------------
BCSplashScreen::BCSplashScreen(QWidget *parent) : QSplashScreen(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
}
//-----------------------------------------------------------------------------
BCSplashScreen::~BCSplashScreen()
{

}
//-----------------------------------------------------------------------------
void BCSplashScreen::mousePressEvent(QMouseEvent *e)
{
	return;
}
//-----------------------------------------------------------------------------
