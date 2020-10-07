#include "ISPopupMessage.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISPopupMessage::ISPopupMessage(const QString &Title, const QString &Message, QWidget *parent) : QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	PropertyAnimation = new QPropertyAnimation(this);
	PropertyAnimation->setTargetObject(this);
	PropertyAnimation->setPropertyName("PopupOpacity");
	connect(PropertyAnimation, &QAbstractAnimation::finished, this, &ISPopupMessage::hide);

	LabelTitle = new QLabel(this);
	LabelTitle->setVisible(false);
	LabelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	LabelTitle->setStyleSheet(BUFFER_STYLE_SHEET("ISPopup"));
	SetTitle(Title);
	
	LabelMessage = new QLabel(Message, this);
	LabelMessage->setVisible(false);
	LabelMessage->setStyleSheet(BUFFER_STYLE_SHEET("ISPopup"));
	SetMessage(Message);

	GridLayout = new QGridLayout(this);
	GridLayout->addWidget(LabelTitle, 0, 0);
	GridLayout->addWidget(LabelMessage, 1, 0);
	setLayout(GridLayout);

	Timer = new QTimer(this);
	connect(Timer, &QTimer::timeout, this, &ISPopupMessage::HideAnimation);
}
//-----------------------------------------------------------------------------
ISPopupMessage::ISPopupMessage(const QString &Message, QWidget *parent) : ISPopupMessage(QString(), Message, parent)
{

}
//-----------------------------------------------------------------------------
ISPopupMessage::ISPopupMessage(QWidget *parent) : ISPopupMessage(QString(), QString(), parent)
{

}
//-----------------------------------------------------------------------------
ISPopupMessage::~ISPopupMessage()
{

}
//-----------------------------------------------------------------------------
void ISPopupMessage::ShowNotification(const QString &Title, const QString &Message)
{
	(new ISPopupMessage(Title, Message))->show();
}
//-----------------------------------------------------------------------------
void ISPopupMessage::ShowNotification(const QString &Mesage)
{
	(new ISPopupMessage(QString(), Mesage))->show();
}
//-----------------------------------------------------------------------------
void ISPopupMessage::SetPopupOpacity(float Opacity)
{
	PopupOpacity = Opacity;
	setWindowOpacity(Opacity);
}
//-----------------------------------------------------------------------------
float ISPopupMessage::GetPopupOpacity() const
{
	return PopupOpacity;
}
//-----------------------------------------------------------------------------
void ISPopupMessage::SetTitleAlignment(Qt::Alignment Alignment)
{
	LabelTitle->setAlignment(Alignment);
}
//-----------------------------------------------------------------------------
void ISPopupMessage::SetMessageAlignment(Qt::Alignment Alignment)
{
	LabelMessage->setAlignment(Alignment);
}
//-----------------------------------------------------------------------------
void ISPopupMessage::SetTitle(const QString &Text)
{
	if (!Text.isEmpty())
	{
		LabelTitle->setVisible(true);
		LabelTitle->setText(Text);
		adjustSize();
	}
}
//-----------------------------------------------------------------------------
void ISPopupMessage::SetMessage(const QString &Message)
{
	if (!Message.isEmpty())
	{
		LabelMessage->setVisible(true);
		LabelMessage->setText(Message);
		adjustSize();
	}
}
//-----------------------------------------------------------------------------
void ISPopupMessage::show()
{
	setWindowOpacity(0.0);

	PropertyAnimation->setDuration(150);
	PropertyAnimation->setStartValue(0.0);
	PropertyAnimation->setEndValue(1.0);

	QWidget::show();
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_RightBottom);

	PropertyAnimation->start();
	Timer->start(4500);
}
//-----------------------------------------------------------------------------
void ISPopupMessage::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QPainter Painter(this);
	Painter.setRenderHint(QPainter::Antialiasing); // Включаем сглаживание

	
	Painter.setBrush(QBrush(ISDefines::Gui::COLOR_POPUP_BRUSH));
	Painter.setPen(Qt::NoPen); // Край уведомления не будет выделен

	QRect RoundedRect;
	RoundedRect.setX(rect().x() + 5);
	RoundedRect.setY(rect().y() + 5);
	RoundedRect.setWidth(rect().width() - 10);
	RoundedRect.setHeight(rect().height() - 10);
	Painter.drawRoundedRect(RoundedRect, 10, 10);
}
//-----------------------------------------------------------------------------
void ISPopupMessage::mousePressEvent(QMouseEvent *MouseEvent)
{
	Q_UNUSED(MouseEvent);
	emit Clicked();
	SetPopupOpacity(0.0);
	hide();
}
//-----------------------------------------------------------------------------
void ISPopupMessage::enterEvent(QEvent *e)
{
	QWidget::enterEvent(e);
	PropertyAnimation->stop();
	SetPopupOpacity(1.0);
	Timer->stop();
}
//-----------------------------------------------------------------------------
void ISPopupMessage::leaveEvent(QEvent *e)
{
	QWidget::leaveEvent(e);
	Timer->start();
}
//-----------------------------------------------------------------------------
void ISPopupMessage::HideAnimation()
{
	Timer->stop();
	PropertyAnimation->setDuration(1000);
	PropertyAnimation->setStartValue(1.0);
	PropertyAnimation->setEndValue(0.0);
	PropertyAnimation->start();
}
//-----------------------------------------------------------------------------
void ISPopupMessage::hide()
{
	if (PopupOpacity == 0.0)
	{
		delete this;
	}
}
//-----------------------------------------------------------------------------
