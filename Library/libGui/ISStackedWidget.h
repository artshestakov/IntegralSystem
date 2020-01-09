#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISStackedWidget : public QStackedWidget
{
	Q_OBJECT

public:
	ISStackedWidget(QWidget *parent = 0);
	virtual ~ISStackedWidget();

	QVBoxLayout* GetMainLayout();
	void SetMainLayout(QLayout *Layout);

	void ActivateMainWidget();
	void ActivateScreenWidget();

	QWidget *MainWidget;
private:
	QWidget *ScreenWidget;
	QLabel *LabelScreen;
	QVBoxLayout *MainLayout;
};
//-----------------------------------------------------------------------------
