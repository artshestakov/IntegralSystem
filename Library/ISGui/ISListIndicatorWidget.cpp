#include "ISListIndicatorWidget.h"
#include "ISDefinesGui.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISListIndicatorWidget::ISListIndicatorWidget(QWidget *parent) : QWidget(parent)
{
	setVisible(false);
	setCursor(CURSOR_WAIT);

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
	LabelText->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
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
	setToolTip(QString());
}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::SetVisibleAnimation(bool Visible)
{
	if (Visible)
	{
		WaitWidget->Start();
	}
	else
	{
		WaitWidget->Stop();
	}

	adjustSize();
}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::SetPixmap(const QPixmap &Pixmap)
{
	if (Pixmap.isNull())
	{
		LabelImage->setPixmap(Pixmap);
	}
	else
	{
		LabelImage->setPixmap(Pixmap.scaled(ISDefines::Gui::SIZE_32_32, Qt::KeepAspectRatio));
	}
	
	adjustSize();
}
//-----------------------------------------------------------------------------
void ISListIndicatorWidget::SetText(const QString &Text)
{
	LabelText->setText(Text);
	adjustSize();
}
//-----------------------------------------------------------------------------
