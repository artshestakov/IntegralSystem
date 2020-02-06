#include "ISSearchEdit.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_SEARCH_FAST = PREPARE_QUERY("SELECT srfs_value "
											  "FROM _searchfast "
											  "WHERE srfs_user = currentuserid() "
											  "ORDER BY srfs_id "
											  "LIMIT :Limit");
//-----------------------------------------------------------------------------
ISSearchEdit::ISSearchEdit(QWidget *parent) : ISLineEdit(parent)
{
	connect(this, &ISSearchEdit::DataChanged, this, &ISSearchEdit::SearchChanged);

	SetPlaceholderText(LANG("Search"));
	SetToolTip(LANG("EnteringSearchQuery"));
	SetIcon(BUFFER_ICONS("Search"));

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_SEARCH_FAST_REMEMBER))
	{
		ButtonLastSearch = new ISServiceButton(this);
		ButtonLastSearch->setText("...");
		ButtonLastSearch->setToolTip(LANG("ClickFromViewMyLLastSearchQuery"));
		ButtonLastSearch->setMenu(new QMenu(ButtonLastSearch));
		connect(ButtonLastSearch->menu(), &QMenu::aboutToShow, this, &ISSearchEdit::AboutToShow);
		connect(ButtonLastSearch->menu(), &QMenu::aboutToHide, this, &ISSearchEdit::AboutToHide);
		AddWidgetToRight(ButtonLastSearch);
	}

	Timer = new QTimer(this);
	Timer->setInterval(800);
	connect(Timer, &QTimer::timeout, this, &ISSearchEdit::Timeout);
}
//-----------------------------------------------------------------------------
ISSearchEdit::~ISSearchEdit()
{

}
//-----------------------------------------------------------------------------
void ISSearchEdit::Updated()
{
	QTimer::singleShot(10, this, &ISSearchEdit::SetFocus);
}
//-----------------------------------------------------------------------------
void ISSearchEdit::SearchChanged()
{
	if (GetValue().toString().length())
	{
		Timer->start();
	}
	else
	{
		emit ClearPressed();
	}
}
//-----------------------------------------------------------------------------
void ISSearchEdit::AboutToShow()
{
	ISGui::SetWaitGlobalCursor(true);

	while (ButtonLastSearch->menu()->actions().count())
	{
		QAction *Action = ButtonLastSearch->menu()->actions().takeAt(0);
		delete Action;
		Action = nullptr;
	}

	ISQuery qSelect(QS_SEARCH_FAST);
	qSelect.BindValue(":Limit", SETTING_INT(CONST_UID_SETTING_TABLES_SEARCH_FAST_VIEW));
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QAction *Action = new QAction(this);
			Action->setText(qSelect.ReadColumn("srfs_value").toString());
			connect(Action, &QAction::triggered, this, &ISSearchEdit::LastSearchClicked);
			ButtonLastSearch->menu()->addAction(Action);
		}
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISSearchEdit::AboutToHide()
{
	
}
//-----------------------------------------------------------------------------
void ISSearchEdit::LastSearchClicked()
{
	QString SearchValue = dynamic_cast<QAction*>(sender())->text();
	SetValue(SearchValue);
}
//-----------------------------------------------------------------------------
void ISSearchEdit::Timeout()
{
	Timer->stop();
	ISGui::RepaintWidget(this);

	emit Search(GetValue().toString());
}
//-----------------------------------------------------------------------------
