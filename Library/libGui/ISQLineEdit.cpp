#include "StdAfx.h"
#include "ISQLineEdit.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISContextMenuText.h"
#include "ISControls.h"
#include "ISProperty.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISQLineEdit::ISQLineEdit(QWidget *parent) : QLineEdit(parent)
{
	MenuCopy = nullptr;

	setStyleSheet(STYLE_SHEET("ISLineEdit"));
	setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	setDragEnabled(true);

	if (ISProperty::GetInstance().GetValueBool(IS_PROPERTY_LINE_EDIT_SELECTED_MENU))
	{
		SetMenuSelected(true);
	}

	ActionClear = new QAction(this);
	ActionClear->setToolTip(LOCALIZATION("Clear.Field"));
	ActionClear->setIcon(BUFFER_ICONS("ButtonClear"));
	connect(ActionClear, &QAction::triggered, this, &ISQLineEdit::ClearClicked);
	AddAction(ActionClear, QLineEdit::TrailingPosition);
}
//-----------------------------------------------------------------------------
ISQLineEdit::~ISQLineEdit()
{

}
//-----------------------------------------------------------------------------
void ISQLineEdit::SetIcon(const QIcon &icon)
{
	Icon = icon;
	if (icon.isNull())
	{
		setTextMargins(0, 0, 0, 0);
	}
	else
	{
		setTextMargins(20, 0, 0, 0);
	}
}
//-----------------------------------------------------------------------------
void ISQLineEdit::SetMenuSelected(bool menu)
{
	if (menu)
	{
		MenuCopy = new QMenu(this);

		QAction *ActionCut = ISControls::GetActionContextCut(MenuCopy);
		connect(ActionCut, &QAction::triggered, this, &ISQLineEdit::cut);
		MenuCopy->addAction(ActionCut);

		QAction *ActionCopy = ISControls::GetActionContextCopy(MenuCopy);
		connect(ActionCopy, &QAction::triggered, this, &ISQLineEdit::copy);
		MenuCopy->addAction(ActionCopy);
	}
	else
	{
		if (MenuCopy)
		{
			delete MenuCopy;
			MenuCopy = nullptr;
		}
	}
}
//-----------------------------------------------------------------------------
void ISQLineEdit::SetVisibleClear(bool visible)
{
	if (ActionClear)
	{
		ActionClear->setVisible(visible);
	}
}
//-----------------------------------------------------------------------------
void ISQLineEdit::AddAction(QAction *Action, QLineEdit::ActionPosition Position)
{
	addAction(Action, Position);
}
//-----------------------------------------------------------------------------
void ISQLineEdit::ContextMenuCopy()
{
	QString SelectedText = selectedText();
	QApplication::clipboard()->setText(SelectedText);
}
//-----------------------------------------------------------------------------
void ISQLineEdit::focusInEvent(QFocusEvent *e)
{
	QLineEdit::focusInEvent(e);
	emit FocusInSignal();
}
//-----------------------------------------------------------------------------
void ISQLineEdit::focusOutEvent(QFocusEvent *e)
{
	QLineEdit::focusOutEvent(e);
	emit FocusOutSignal();
}
//-----------------------------------------------------------------------------
void ISQLineEdit::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuText ContextMenu(this, isReadOnly(), isUndoAvailable(), isRedoAvailable(), hasSelectedText(), echoMode(), text().isEmpty(), selectedText().count());
	connect(&ContextMenu, &ISContextMenuText::Delete, this, &ISQLineEdit::del);
	connect(&ContextMenu, &ISContextMenuText::UppercaseText, [=]
	{
		int SelectionStart = selectionStart();
		QString Text = text();
		QString UpperText = selectedText().toUpper();

		setText(Text.replace(SelectionStart, UpperText.length(), UpperText));
		setSelection(SelectionStart, UpperText.length());
	});
	connect(&ContextMenu, &ISContextMenuText::LowercaseText, [=]
	{
		int SelectionStart = selectionStart();
		QString Text = text();
		QString LowerText = selectedText().toLower();

		setText(Text.replace(SelectionStart, LowerText.length(), LowerText));
		setSelection(SelectionStart, LowerText.length());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchGoogle, [=]
	{
		ISGui::OpenUrl("https://www.google.ru/search?q=" + text());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchYandex, [=]
	{
		ISGui::OpenUrl("https://www.yandex.ru/search/?text=" + text());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchWikipedia, [=]
	{
		ISGui::OpenUrl("https://ru.wikipedia.org/wiki/Special:Search?search=" + text());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchMail, [=]
	{
		ISGui::OpenUrl("https://go.mail.ru/search?q=" + text());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchBing, [=]
	{
		ISGui::OpenUrl("https://www.bing.com/search?q=" + text());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchYahoo, [=]
	{
		ISGui::OpenUrl("https://search.yahoo.com/search?p=" + text());
	});
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
void ISQLineEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	QLineEdit::mouseDoubleClickEvent(e);
	emit MouseDoubleClicked();
}
//-----------------------------------------------------------------------------
void ISQLineEdit::mousePressEvent(QMouseEvent *e)
{
	QLineEdit::mousePressEvent(e);

	if (e->button() == Qt::MiddleButton)
	{
		if (!isReadOnly())
		{
			paste();
		}
	}

	emit MousePressEvent();
}
//-----------------------------------------------------------------------------
void ISQLineEdit::mouseReleaseEvent(QMouseEvent *e)
{
	QLineEdit::mouseReleaseEvent(e);
	
	if (MenuCopy)
	{
		if (e->button() == Qt::LeftButton)
		{
			if (selectedText().length())
			{
				MenuCopy->exec(e->globalPos());
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISQLineEdit::wheelEvent(QWheelEvent *e)
{
	QLineEdit::wheelEvent(e);
	emit WheelEventSignal(e);
}
//-----------------------------------------------------------------------------
void ISQLineEdit::keyPressEvent(QKeyEvent *e)
{
	QLineEdit::keyPressEvent(e);
	emit KeyPressEventSignal(e);
}
//-----------------------------------------------------------------------------
void ISQLineEdit::paintEvent(QPaintEvent *e)
{
	QLineEdit::paintEvent(e);

	if (!Icon.isNull())
	{
		QPainter Painter(this);
		QPixmap Pixmap = Icon.pixmap(height() - 6, height() - 6);
		int cx = Pixmap.width();

		Painter.drawPixmap(2, 3, Pixmap);
		Painter.setPen(Qt::lightGray);
		Painter.drawLine(cx + 2, 3, cx + 2, height() - 4);
	}
}
//-----------------------------------------------------------------------------
