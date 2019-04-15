#include "StdAfx.h"
#include "ISSplashWidget.h"
#include "EXDefines.h"
#include "ISSystem.h"
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
	ISSystem::RepaintWidget(this);
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISSplashWidget::SetText(const QString &Text)
{
	LabelText->setText(Text);
	ISSystem::RepaintWidget(LabelText);
	adjustSize();
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
