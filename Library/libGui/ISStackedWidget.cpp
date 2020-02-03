#include "ISStackedWidget.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISStackedWidget::ISStackedWidget(QWidget *parent) : QStackedWidget(parent)
{
	MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(MARGINS_LAYOUT_NULL);

	MainWidget = new QWidget(this);
	MainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	MainWidget->setLayout(MainLayout);
	addWidget(MainWidget);

	ScreenWidget = new QWidget(this);
	ScreenWidget->setLayout(new QVBoxLayout());
	ScreenWidget->layout()->setContentsMargins(MARGINS_LAYOUT_NULL);
	addWidget(ScreenWidget);

	LabelScreen = new QLabel(ScreenWidget);
	ScreenWidget->layout()->addWidget(LabelScreen);
}
//-----------------------------------------------------------------------------
ISStackedWidget::~ISStackedWidget()
{

}
//-----------------------------------------------------------------------------
QVBoxLayout* ISStackedWidget::GetMainLayout()
{
	return MainLayout;
}
//-----------------------------------------------------------------------------
void ISStackedWidget::SetMainLayout(QLayout *Layout)
{
	MainWidget->setLayout(Layout);
}
//-----------------------------------------------------------------------------
void ISStackedWidget::ActivateMainWidget()
{
	LabelScreen->clear();
	setCurrentWidget(MainWidget);
}
//-----------------------------------------------------------------------------
void ISStackedWidget::ActivateScreenWidget()
{
	QPixmap Pixmap = QWidget::grab(MainWidget->rect());
	QImage Image(Pixmap.size(), QImage::Format_ARGB32);
	
	QPainter Painter;
	Painter.begin(&Image);
	Painter.drawPixmap(0, 0, Pixmap);
	Painter.fillRect(Image.rect(), COLOR_MAIN_WINDOW_SHADOW);
	Painter.end();

	LabelScreen->setPixmap(QPixmap::fromImage(Image));
	setCurrentWidget(ScreenWidget);
}
//-----------------------------------------------------------------------------
