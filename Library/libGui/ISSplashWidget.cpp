#include "StdAfx.h"
#include "ISSplashWidget.h"
#include "ISDefines.h"
#include "ISGui.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISSplashWidget::ISSplashWidget(QWidget *parent) : QFrame(parent)
{
	setFrameShape(QFrame::Box);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setCursor(CURSOR_WAIT);

	QHBoxLayout *Layout = new QHBoxLayout();
	setLayout(Layout);

	LabelText = new QLabel(this);
	Layout->addWidget(LabelText);
}
//-----------------------------------------------------------------------------
ISSplashWidget::~ISSplashWidget()
{

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
void ISSplashWidget::SetText(const QString &Text)
{
	LabelText->setText(Text);
	ISGui::RepaintWidget(LabelText);
	adjustSize();
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
