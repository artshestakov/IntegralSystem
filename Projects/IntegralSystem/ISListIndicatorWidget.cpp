#include "ISListIndicatorWidget.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISListIndicatorWidget::ISListIndicatorWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *Layout = new QHBoxLayout();
    setLayout(Layout);

    WaitWidget = new ISWaitWidget(this, false, false);
    WaitWidget->SetRoundness(70);
    WaitWidget->SetMinimumTrailOpacity(15);
    WaitWidget->SetTrailFadePercentage(70);
    WaitWidget->SetNumberOfLines(14);
    WaitWidget->SetLineLength(15);
    WaitWidget->SetLineWidth(3);
    WaitWidget->SetInnerRadius(7);
    WaitWidget->SetRevolutionsPerSecond(2);
    Layout->addWidget(WaitWidget);

    LabelImage = new QLabel(this);
    Layout->addWidget(LabelImage);

    LabelText = new QLabel(this);
    LabelText->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
    LabelText->setFont(ISDefines::Gui::FONT_TAHOMA_14_BOLD);
    Layout->addWidget(LabelText);
}
//-----------------------------------------------------------------------------
ISListIndicatorWidget::~ISListIndicatorWidget()
{

}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
    WaitWidget->Stop();
    LabelImage->setPixmap(QPixmap());
    LabelText->clear();
    setToolTip(QString());
}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::SetVisibleAnimation(bool Visible)
{
    Visible ? WaitWidget->Start() : WaitWidget->Stop();
    adjustSize();
}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::SetIcon(const QIcon &Icon)
{
    LabelImage->setPixmap(Icon.isNull() ? QPixmap() : Icon.pixmap(ISDefines::Gui::SIZE_32_32));
    adjustSize();
}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::SetText(const QString &Text)
{
    LabelText->setText(Text);
    adjustSize();
}
//-----------------------------------------------------------------------------
