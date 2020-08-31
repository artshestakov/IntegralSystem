#include "ISSplashWidget.h"
#include "ISDefinesGui.h"
#include "ISGui.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISSplashWidget::ISSplashWidget(QWidget *parent) : QFrame(parent)
{
	setFrameShape(QFrame::Box);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setCursor(CURSOR_WAIT);

	QHBoxLayout *Layout = new QHBoxLayout();
	setLayout(Layout);

	QLabel *LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(QPixmap(":Images/SplashWidgetLogo.png"));
	Layout->addWidget(LabelIcon);

	LabelText = new QLabel(this);
	Layout->addWidget(LabelText);

	SetText("IntegralSystem");
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
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISSplashWidget::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);

	QWidget::show();
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
	update();
	adjustSize();
	ISGui::RepaintWidget(this);
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
