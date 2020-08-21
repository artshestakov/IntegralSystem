#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISButtons.h"
#include "ISUuid.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISMenuBar : public QWidget
{
	Q_OBJECT

signals:
	void ParagraphClicked(const ISUuid &ParagraphUID);
	void ChangeUser();
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

	void ParagraphClick(const ISUuid &ParagraphUID);

private:
	QToolButton* CreateButton(const QString &ToolTip, const QString &IconName);
	void ParagraphClicked();
	QToolButton* CreateParagraphButton(ISMetaParagraph *MetaParagraph);

private:
	QHBoxLayout *LayoutButtons;
	ISButtonMainMenu *ButtonMenu;
	std::map<ISUuid, QToolButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
