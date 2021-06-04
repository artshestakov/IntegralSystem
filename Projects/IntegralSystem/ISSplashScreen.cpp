#include "ISSplashScreen.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstantsOld.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISSplashScreen::ISSplashScreen(const QString &Message)
    : QWidget(nullptr, Qt::SplashScreen | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
{
    setFont(QFont("Tahoma", 12));
    setCursor(CURSOR_WAIT);

    QPixmap Pixmap = BUFFER_PIXMAPS("BannerSplashScreen");
    setFixedSize(Pixmap.size());

    QPalette Palette;
    Palette.setBrush(QPalette::Background, Pixmap);
    setPalette(Palette);

    QVBoxLayout *MainLayout = new QVBoxLayout();
    MainLayout->setContentsMargins(QMargins(5, 5, 5, 5));
    MainLayout->addStretch();
    setLayout(MainLayout);

    LabelText = new QLabel(this);
    LabelText->setText(Message);
    MainLayout->addWidget(LabelText, 0, Qt::AlignCenter);

    QColor ColorLabelText(37, 77, 158);

    QPalette PaletteLabel = LabelText->palette();
    PaletteLabel.setColor(LabelText->backgroundRole(), ColorLabelText);
    PaletteLabel.setColor(LabelText->foregroundRole(), ColorLabelText);
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
