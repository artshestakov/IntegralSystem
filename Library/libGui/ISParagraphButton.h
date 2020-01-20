#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMetaParagraph.h"
//-----------------------------------------------------------------------------
class ISParagraphButton : public QWidget
{
	Q_OBJECT

signals:
	void Clicked();

public:
	ISParagraphButton(ISMetaParagraph *MetaParagraph, QWidget *parent = 0);
	virtual ~ISParagraphButton();

	void SetText(const QString &Text);
	void SetToolTip(const QString &ToolTip);
	void SetCursor(const QCursor &Cursor);
	void SetVisibleLine(bool Visible);

private:
	QToolButton *ToolButton;

	QWidget *LineLeft;
	QLabel *LineCenter;
	QWidget *LineRight;
};
//-----------------------------------------------------------------------------
