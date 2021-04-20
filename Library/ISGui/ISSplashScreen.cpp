#include "ISSplashScreen.h"
#include "ISDefinesGui.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISSplashScreen::ISSplashScreen(const QString &Message)
    : QWidget(nullptr, Qt::SplashScreen | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
{
    setFont(ISDefines::Gui::FONT_TAHOMA_12);
    setCursor(CURSOR_WAIT);

    QPixmap Pixmap = BUFFER_PIXMAPS("BannerSplashScreen");
    setFixedSize(Pixmap.size());

    QPalette Palette;
    Palette.setBrush(QPalette::Background, Pixmap);
    setPalette(Palette);

    QVBoxLayout *MainLayout = new QVBoxLayout();
    MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
    MainLayout->addStretch();
    setLayout(MainLayout);

    LabelText = new QLabel(this);
    LabelText->setText(Message);
    MainLayout->addWidget(LabelText, 0, Qt::AlignCenter);

    QPalette PaletteLabel = LabelText->palette();
    PaletteLabel.setColor(LabelText->backgroundRole(), ISDefines::Gui::COLOR_SPLASH_SCREEN_TEXT);
    PaletteLabel.setColor(LabelText->foregroundRole(), ISDefines::Gui::COLOR_SPLASH_SCREEN_TEXT);
    LabelText->setPalette(PaletteLabel);
}
//-----------------------------------------------------------------------------
ISSplashScreen::~ISSplashScreen()
{

}
//-----------------------------------------------------------------------------
void ISSplashScreen::SetMessage(const QString &Message)
{
    LabelText->setText(Message);
    LabelText->adjustSize();
    ISGui::RepaintWidget(this);
}
//-----------------------------------------------------------------------------
