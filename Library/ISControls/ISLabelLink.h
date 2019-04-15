#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISQLabel.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISLabelLink : public ISQLabel
{
	Q_OBJECT

public:
	ISLabelLink(const QString &Text, QWidget *parent = 0);
	ISLabelLink(QWidget *parent = 0);
	virtual ~ISLabelLink();

protected:
	void keyPressEvent(QKeyEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
};
//-----------------------------------------------------------------------------
