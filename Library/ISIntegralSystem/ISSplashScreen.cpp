#include "StdAfx.h"
#include "ISSplashScreen.h"
#include "ISAssert.h"
#include "EXDefines.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISSplashScreen::ISSplashScreen(QWidget *parent) : QSplashScreen(parent)
{
	setFont(FONT_TAHOMA_12);
	setCursor(CURSOR_WAIT);
	DefaultPixmap();

	QVBoxLayout *MainLayout = new QVBoxLayout();
	MainLayout->setSpacing(0);
	MainLayout->addStretch();
	setLayout(MainLayout);

	QLabel *LabelCopyright = new QLabel(this);
	LabelCopyright->setFont(FONT_TAHOMA_8);
	LabelCopyright->setText(LOCALIZATION("SplashSceen.Copyright"));
	MainLayout->addWidget(LabelCopyright, 0, Qt::AlignRight);

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(0, 0, 0, 15);
	MainLayout->addLayout(Layout);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setValue(0);
	ProgressBar->setRange(0, 100);
	ProgressBar->setTextVisible(false);
	ProgressBar->setStyleSheet(STYLE_SHEET("Banner.ProgressBar"));
	ProgressBar->setMaximumHeight(25);
	Layout->addWidget(ProgressBar);
}
//-----------------------------------------------------------------------------
ISSplashScreen::~ISSplashScreen()
{

}
//-----------------------------------------------------------------------------
ISSplashScreen& ISSplashScreen::GetInstance()
{
	static ISSplashScreen SplashScreen;
	return SplashScreen;
}
//-----------------------------------------------------------------------------
void ISSplashScreen::SetMessage(const QString &Message)
{
	int Value = ProgressBar->value();
	if (Value == 100)
	{
		Value = 0;
	}
	else
	{
		Value = Value + 5;
	}
	ProgressBar->setValue(Value);

	ISDebug::ShowInfoString(Message);
	showMessage(Message, Qt::AlignHCenter | Qt::AlignBottom, COLOR_SPLASH_SCREEN_TEXT);
	activateWindow();

	ISSystem::RepaintWidget(ProgressBar, false);
	ISSystem::RepaintWidget(this, false);
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISSplashScreen::ResetPixmap()
{
	setPixmap(QPixmap());
}
//-----------------------------------------------------------------------------
void ISSplashScreen::DefaultPixmap()
{
	setPixmap(BUFFER_PIXMAPS("BannerSplashScreen"));
}
//-----------------------------------------------------------------------------
void ISSplashScreen::mousePressEvent(QMouseEvent *e)
{
	//Это переопределение нужно для того, чтобы банер не скрывался при нажатии кнопок мыши
	return;
}
//-----------------------------------------------------------------------------
