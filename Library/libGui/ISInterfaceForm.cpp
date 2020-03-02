#include "ISInterfaceForm.h"
#include "ISDefinesGui.h"
#include "ISControls.h"
#include "ISSystem.h"
#include "ISStyleSheet.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISInterfaceForm::ISInterfaceForm(QWidget *parent, Qt::WindowFlags Flags)
	: QWidget(parent, Flags),
	LabelShadow(nullptr),
	AnimationShow(nullptr),
	FlashingTimer(nullptr),
	ShowedFlag(false),
	FormUID(ISSystem::GenerateUuid())
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAutoFillBackground(true);
	
	MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(MainLayout);

	ISControls::SetBackgroundColorWidget(this, ISDefines::Gui::COLOR_WHITE);

	QAction *ActionWindowState = new QAction(this);
	ActionWindowState->setShortcut(Qt::Key_F11);
	connect(ActionWindowState, &QAction::triggered, this, &ISInterfaceForm::SetWindowState);
	addAction(ActionWindowState);

	QAction *ActionEscape = new QAction(this);
	ActionEscape->setShortcut(Qt::Key_Escape);
	connect(ActionEscape, &QAction::triggered, this, &ISInterfaceForm::EscapeClicked);
	addAction(ActionEscape);

	QAction *ActionEnter = new QAction(this);
	ActionEnter->setShortcut(Qt::Key_Enter);
	connect(ActionEnter, &QAction::triggered, this, &ISInterfaceForm::EnterClicked);
	addAction(ActionEnter);

	QAction *ActionReturn = new QAction(this);
	ActionReturn->setShortcut(Qt::Key_Return);
	connect(ActionReturn, &QAction::triggered, this, &ISInterfaceForm::EnterClicked);
	addAction(ActionReturn);

	QAction *ActionControlEnter = new QAction(this);
	ActionControlEnter->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Enter));
	connect(ActionControlEnter, &QAction::triggered, this, &ISInterfaceForm::ControlEnterClicked);
	addAction(ActionControlEnter);

	QAction *ActionControlReturn = new QAction(this);
	ActionControlReturn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return));
	connect(ActionControlReturn, &QAction::triggered, this, &ISInterfaceForm::ControlEnterClicked);
	addAction(ActionControlReturn);

	AnimationShow = new QPropertyAnimation(this, "windowOpacity", this);
	connect(AnimationShow, &QPropertyAnimation::finished, this, &ISInterfaceForm::show);

	AnimationHide = new QPropertyAnimation(this, "windowOpacity", this);
	connect(AnimationHide, &QPropertyAnimation::finished, this, &ISInterfaceForm::hide);
}
//-----------------------------------------------------------------------------
ISInterfaceForm::~ISInterfaceForm()
{
	
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::ShowAnimated(bool maximized, int duration)
{
	setWindowOpacity(0.0);
	
	if (maximized)
	{
		showMaximized();
	}
	else
	{
		show();
	}

	AnimationShow->setStartValue(windowOpacity());
	AnimationShow->setEndValue(1.0);

	if (duration)
	{
		AnimationShow->setDuration(duration);
	}
	else
	{
		AnimationShow->setDuration(DURATION_SHOW_HIDE_ANIMATION);
	}

	AnimationShow->start();
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::HideAnimation(int duration)
{
	setWindowOpacity(1.0);

	AnimationHide->setStartValue(windowOpacity());
	AnimationHide->setEndValue(0.0);

	if (duration)
	{
		AnimationHide->setDuration(duration);
	}
	else
	{
		AnimationHide->setDuration(DURATION_SHOW_HIDE_ANIMATION);
	}

	AnimationHide->start();
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::SetVisibleShadow(bool Visible)
{
	if (Visible && LabelShadow)
	{
		return;
	}

	if (Visible)
	{
		LabelShadow = new QLabel(this);
		LabelShadow->setStyleSheet(STYLE_SHEET("QLabel.Shadow"));
		LabelShadow->move(0, 0);
		LabelShadow->resize(width(), height());
		LabelShadow->show();
	}
	else
	{
		delete LabelShadow;
		LabelShadow = nullptr;
	}
}
//-----------------------------------------------------------------------------
bool ISInterfaceForm::IsVisibleShadow() const
{
	return LabelShadow;
}
//-----------------------------------------------------------------------------
ISUuid ISInterfaceForm::GetFormUID() const
{
	return FormUID;
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	if (!ShowedFlag)
	{
		ShowedFlag = true;
		AfterShowEvent();
	}
	emit Showed();
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	if (LabelShadow)
	{
		LabelShadow->resize(width(), height());
	}
}
//-----------------------------------------------------------------------------
QVBoxLayout* ISInterfaceForm::GetMainLayout()
{
	return MainLayout;
}
//-----------------------------------------------------------------------------
bool ISInterfaceForm::IsShowed() const
{
	return ShowedFlag;
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::ForbidResize()
{
	if (!parentWidget())
	{
		setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	}
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::FlashingStart(int Interval, const QColor &Color)
{
	if (!FlashingTimer)
	{
		FlashingTimer = new QTimer(this);
		FlashingTimer->setProperty("Color", Color);
		FlashingTimer->setProperty("AltColor", false);
		connect(FlashingTimer, &QTimer::timeout, this, &ISInterfaceForm::FlashingTimeout);
		FlashingTimer->start(Interval);
	}
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::FlashingTimeout()
{
	bool AltColor = FlashingTimer->property("AltColor").toBool();
	ISControls::SetBackgroundColorWidget(this, AltColor ? ISDefines::Gui::COLOR_WHITE : qvariant_cast<QColor>(FlashingTimer->property("Color")));
	FlashingTimer->setProperty("AliColor", !AltColor);
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::FlashingStop()
{
	if (FlashingTimer)
	{
		ISControls::SetBackgroundColorWidget(this, ISDefines::Gui::COLOR_WHITE);
		FlashingTimer->stop();
		
		ISSystem::ExecLoop(100);
		delete FlashingTimer;
		FlashingTimer = nullptr;
	}
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::AfterShowEvent()
{
	
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::SetWindowState()
{
	switch (windowState())
	{
	case Qt::WindowNoState: setWindowState(Qt::WindowMaximized); break;
	case Qt::WindowMaximized: setWindowState(Qt::WindowNoState); break;
	}
}
//-----------------------------------------------------------------------------
void ISInterfaceForm::EscapeClicked()
{

}
//-----------------------------------------------------------------------------
void ISInterfaceForm::EnterClicked()
{

}
//-----------------------------------------------------------------------------
void ISInterfaceForm::ControlEnterClicked()
{

}
//-----------------------------------------------------------------------------
