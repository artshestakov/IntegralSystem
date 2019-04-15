#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISQComboBox : public QComboBox
{
	Q_OBJECT

signals:
	void ShowPopup();
	void HidePopup();

public:
	ISQComboBox(QWidget *parent = 0);
	virtual ~ISQComboBox();

	void SetWheelScroll(bool wheel_scroll);
	bool GetWheelScrool() const;

	void showPopup();
	void hidePopup();
	void wheelEvent(QWheelEvent *e);

protected:
	bool WheelScroll;
};
//-----------------------------------------------------------------------------
