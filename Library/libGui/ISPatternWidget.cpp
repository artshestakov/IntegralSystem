#include "ISPatternWidget.h"
#include "EXDefines.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISPatternWidget::ISPatternWidget(QWidget *parent) : QFrame(parent)
{
	setFrameShape(QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setAutoFillBackground(true);

	QVBoxLayout *MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(MARGINS_LAYOUT_5_PX);
	setLayout(MainLayout);

	LabelIcon = new QLabel(this);
	LabelIcon->setAlignment(Qt::AlignCenter);
	MainLayout->addWidget(LabelIcon, 0, Qt::AlignCenter);

	LabelPattern = new QLabel(this);
	LabelPattern->setFont(FONT_TAHOMA_12_BOLD);
	LabelPattern->setAlignment(Qt::AlignCenter);
	LabelPattern->setStyleSheet(STYLE_SHEET("QLabel.Color.White"));
	MainLayout->addWidget(LabelPattern, 0, Qt::AlignCenter);

	SetIcon(BUFFER_ICONS("TelephonyForm.Activity.Status.4"));
	SetBackgroundColor(COLOR_PATTERN_WIDGET_NOT_REGISTERED);
}
//-----------------------------------------------------------------------------
ISPatternWidget::~ISPatternWidget()
{

}
//-----------------------------------------------------------------------------
void ISPatternWidget::SetIcon(const QIcon &Icon)
{
	LabelIcon->setPixmap(Icon.pixmap(SIZE_32_32));
}
//-----------------------------------------------------------------------------
void ISPatternWidget::SetPattern(const QString &Pattern)
{
	LabelPattern->setText(Pattern);
}
//-----------------------------------------------------------------------------
void ISPatternWidget::SetBackgroundColor(const QString &BackgroundColor)
{
	QColor Color = ISGui::StringToColor(BackgroundColor);
	QPalette Palette(palette());
	Palette.setColor(QPalette::Background, Color);
	setPalette(Palette);
}
//-----------------------------------------------------------------------------
void ISPatternWidget::enterEvent(QEvent *e)
{
	QWidget::enterEvent(e);
	setFrameShape(QFrame::WinPanel);
}
//-----------------------------------------------------------------------------
void ISPatternWidget::leaveEvent(QEvent *e)
{
	QWidget::leaveEvent(e);
	setFrameShape(QFrame::Box);
}
//-----------------------------------------------------------------------------
