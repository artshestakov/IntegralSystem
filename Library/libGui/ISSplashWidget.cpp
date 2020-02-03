#include "ISSplashWidget.h"
#include "ISDefines.h"
#include "ISGui.h"
#include "ISDebug.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISSplashWidget::ISSplashWidget(QWidget *parent) : QFrame(parent)
{
	setFrameShape(QFrame::Box);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setCursor(CURSOR_WAIT);

	QHBoxLayout *Layout = new QHBoxLayout();
	setLayout(Layout);

	QLabel *LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS("Add").pixmap(SIZE_64_64));
	Layout->addWidget(LabelIcon);

	LabelText = new QLabel(this);
	Layout->addWidget(LabelText);
}
//-----------------------------------------------------------------------------
ISSplashWidget::~ISSplashWidget()
{

}
//-----------------------------------------------------------------------------
void ISSplashWidget::SetText(const QString &Text)
{
	LabelText->setText(Text);
	ISGui::RepaintWidget(LabelText);
	adjustSize();
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISSplashWidget::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);

	QWidget::show();
	update();
	adjustSize();
	ISGui::RepaintWidget(this);
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
