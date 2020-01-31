#include "StdAfx.h"
#include "ISContextMenuText.h"
#include "ISControls.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISContextMenuText::ISContextMenuText(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int SelectedTextCount)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionToUpper = ISControls::GetActionContextToUpper(this);
	ActionToUpper->setEnabled(!ReadOnly && SelectedTextCount);
	connect(ActionToUpper, &QAction::triggered, this, &ISContextMenuText::UppercaseText);
	addAction(ActionToUpper);

	QAction *ActionToLower = ISControls::GetActionContextToLower(this);
	ActionToLower->setEnabled(!ReadOnly && SelectedTextCount);
	connect(ActionToLower, &QAction::triggered, this, &ISContextMenuText::LowercaseText);
	addAction(ActionToLower);

	addSeparator();

	QAction *ActionSearch = new QAction(this);
	ActionSearch->setEnabled(!ReadOnly && !Empty && EchoMode == QLineEdit::Normal);
	ActionSearch->setText(LANG("SearchIn"));
	ActionSearch->setMenu(new QMenu(this));
	addAction(ActionSearch);

	QAction *ActionGoogle = ISControls::GetActionContextGoogle(this);
	connect(ActionGoogle, &QAction::triggered, this, &ISContextMenuText::SearchGoogle);
	ActionSearch->menu()->addAction(ActionGoogle);

	QAction *ActionYandex = ISControls::GetActionContextYandex(this);
	connect(ActionYandex, &QAction::triggered, this, &ISContextMenuText::SearchYandex);
	ActionSearch->menu()->addAction(ActionYandex);

	QAction *ActionWikipedia = ISControls::GetActionContextWikipedia(this);
	connect(ActionWikipedia, &QAction::triggered, this, &ISContextMenuText::SearchWikipedia);
	ActionSearch->menu()->addAction(ActionWikipedia);

	QAction *ActionMail = ISControls::GetActionContextMail(this);
	connect(ActionMail, &QAction::triggered, this, &ISContextMenuText::SearchMail);
	ActionSearch->menu()->addAction(ActionMail);

	QAction *ActionBing = ISControls::GetActionContextBing(this);
	connect(ActionBing, &QAction::triggered, this, &ISContextMenuText::SearchBing);
	ActionSearch->menu()->addAction(ActionBing);

	QAction *ActionYahoo = ISControls::GetActionContextYahoo(this);
	connect(ActionYahoo, &QAction::triggered, this, &ISContextMenuText::SearchYahoo);
	ActionSearch->menu()->addAction(ActionYahoo);
}
//-----------------------------------------------------------------------------
ISContextMenuText::~ISContextMenuText()
{

}
//-----------------------------------------------------------------------------
