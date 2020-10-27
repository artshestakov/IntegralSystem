#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISMenuBar : public QWidget
{
	Q_OBJECT

signals:
	void ParagraphClicked(const ISUuid &ParagraphUID);
	void ChangeUser();
	void RollUp();
	void Exit();
	void Favorites();
	void History();
	void ChangePassword();
	void Settings();
	void AboutApplication();
	void AboutQt();

public:
	ISMenuBar(QWidget *parent = 0);
	virtual ~ISMenuBar();

private:
	QToolButton* CreateButton(const QString &ToolTip, const QString &IconName);
	void ParagraphClicked();
	QToolButton* CreateParagraphButton(ISMetaParagraph *MetaParagraph);

private:
	QHBoxLayout *LayoutButtons;
	std::map<ISUuid, QToolButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
