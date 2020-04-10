#include "ISSplashScreen.h"
#include "ISDefinesGui.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISSplashScreen::ISSplashScreen() : QSplashScreen()
{
	setFont(ISDefines::Gui::FONT_TAHOMA_12);
	setCursor(CURSOR_WAIT);
	DefaultPixmap();

	QVBoxLayout *MainLayout = new QVBoxLayout();
	MainLayout->setSpacing(0);
	MainLayout->addStretch();
	setLayout(MainLayout);

	QLabel *LabelCopyright = new QLabel(this);
	LabelCopyright->setFont(ISDefines::Gui::FONT_TAHOMA_8);
	LabelCopyright->setText(LANG("SplashSceen.Copyright"));
	MainLayout->addWidget(LabelCopyright, 0, Qt::AlignCenter);

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(0, 0, 0, 35);
	MainLayout->addLayout(Layout);
}
//-----------------------------------------------------------------------------
ISSplashScreen::~ISSplashScreen()
{

}
//-----------------------------------------------------------------------------
void ISSplashScreen::SetMessage(const QString &Message)
{
	showMessage(Message, Qt::AlignHCenter | Qt::AlignBottom, ISDefines::Gui::COLOR_SPLASH_SCREEN_TEXT);
	activateWindow();
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
	return; //Это переопределение нужно для того, чтобы банер не скрывался при нажатии кнопок мыши
}
//-----------------------------------------------------------------------------
