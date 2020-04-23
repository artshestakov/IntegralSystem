#include "ISMainMenu.h"
//-----------------------------------------------------------------------------
ISMainMenu::ISMainMenu(QWidget *parent) : QMenu(parent)
{
	setWindowOpacity(0.0);

	PropertyAnimation = new QPropertyAnimation(this, "windowOpacity", this);
	PropertyAnimation->setDuration(220);
	PropertyAnimation->setStartValue(0.0);
	PropertyAnimation->setEndValue(1.0);
}
//-----------------------------------------------------------------------------
ISMainMenu::~ISMainMenu()
{

}
//-----------------------------------------------------------------------------
void ISMainMenu::Show(const QPoint &Point)
{
	move(QPoint(Point.x(), Point.y() + dynamic_cast<QWidget*>(parent())->height()));
	show();
	PropertyAnimation->start();
}
//-----------------------------------------------------------------------------
