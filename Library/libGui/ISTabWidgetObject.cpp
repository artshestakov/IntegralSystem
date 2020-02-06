#include "ISTabWidgetObject.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISTabWidgetObject::ISTabWidgetObject(QWidget *parent) : QTabWidget(parent)
{
	setDocumentMode(true);
	setTabBarAutoHide(true);
}
//-----------------------------------------------------------------------------
ISTabWidgetObject::~ISTabWidgetObject()
{

}
//-----------------------------------------------------------------------------
void ISTabWidgetObject::tabInserted(int Index)
{
	QTabWidget::tabInserted(Index);

	if (Index)
	{
		QToolButton *ButtonClose = new QToolButton(this);
		ButtonClose->setToolTip(LANG("CloseTab"));
		ButtonClose->setIcon(BUFFER_ICONS("Exit"));
		ButtonClose->setAutoRaise(true);
		ButtonClose->setFixedSize(DEFINES_GUI.SIZE_16_16);
		ButtonClose->setProperty("Index", Index);
		connect(ButtonClose, &QToolButton::clicked, this, &ISTabWidgetObject::CloseTab);
		tabBar()->setTabButton(Index, QTabBar::RightSide, ButtonClose);
	}
}
//-----------------------------------------------------------------------------
void ISTabWidgetObject::CloseTab()
{
	int Index = sender()->property("Index").toInt();
	widget(Index)->close();
}
//-----------------------------------------------------------------------------
