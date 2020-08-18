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

	ISParagraphButton* GetParagraphButton(const ISUuid &ParagraphUID);
	void ButtonParagraphClicked(const ISUuid &ClickedParagraphUID);

private:
	QToolButton* CreateButton(const QString &ToolTip, const QString &IconName);
	void ButtonParagraphClicked();

private:
	QHBoxLayout *LayoutButtons;
	ISButtonMainMenu *ButtonMenu;
	QMap<ISUuid, ISParagraphButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
