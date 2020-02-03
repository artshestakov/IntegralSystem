#include "ISQTextEdit.h"
#include "ISContextMenuText.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISQTextEdit::ISQTextEdit(QWidget *parent) : QTextEdit(parent)
{
	Wheel = false;
	ExecEnter = true;

	setStyleSheet(STYLE_SHEET("ISTextEdit"));
}
//-----------------------------------------------------------------------------
ISQTextEdit::~ISQTextEdit()
{

}
//-----------------------------------------------------------------------------
void ISQTextEdit::SetExecuteEnter(bool Enable)
{
	ExecEnter = Enable;
}
//-----------------------------------------------------------------------------
void ISQTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuText ContextMenu(this, isReadOnly(), document()->isUndoAvailable(), document()->isRedoAvailable(), textCursor().hasSelection(), QLineEdit::Normal, toPlainText().isEmpty(), textCursor().selectedText().count());
	connect(&ContextMenu, &ISContextMenuText::Delete, [=] { textCursor().removeSelectedText(); });
	connect(&ContextMenu, &ISContextMenuText::UppercaseText, [=]
	{
		int SelectionStart = textCursor().selectionStart();
		int SelectionEnd = textCursor().selectionEnd();

		QString Text = toPlainText();
		QString SelectedText = textCursor().selectedText();
		QString UpperText = SelectedText.toUpper();

		Text.replace(SelectionStart, UpperText.length(), UpperText);
		setText(Text);

		QTextCursor TextCursor = textCursor();
		TextCursor.setPosition(SelectionStart);
		TextCursor.setPosition(SelectionEnd, QTextCursor::KeepAnchor);
		setTextCursor(TextCursor);
	});
	connect(&ContextMenu, &ISContextMenuText::LowercaseText, [=]
	{
		int SelectionStart = textCursor().selectionStart();
		int SelectionEnd = textCursor().selectionEnd();

		QString Text = toPlainText();
		QString SelectedText = textCursor().selectedText();
		QString LowerText = SelectedText.toLower();

		Text.replace(SelectionStart, LowerText.length(), LowerText);
		setText(Text);

		QTextCursor TextCursor = textCursor();
		TextCursor.setPosition(SelectionStart);
		TextCursor.setPosition(SelectionEnd, QTextCursor::KeepAnchor);
		setTextCursor(TextCursor);
	});
	connect(&ContextMenu, &ISContextMenuText::SearchGoogle, [=]
	{
		ISGui::OpenUrl("https://www.google.ru/search?q=" + toPlainText());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchYandex, [=]
	{
		ISGui::OpenUrl("https://www.yandex.ru/search/?text=" + toPlainText());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchWikipedia, [=]
	{
		ISGui::OpenUrl("https://ru.wikipedia.org/wiki/Special:Search?search=" + toPlainText());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchMail, [=]
	{
		ISGui::OpenUrl("https://go.mail.ru/search?q=" + toPlainText());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchBing, [=]
	{
		ISGui::OpenUrl("https://www.bing.com/search?q=" + toPlainText());
	});
	connect(&ContextMenu, &ISContextMenuText::SearchYahoo, [=]
	{
		ISGui::OpenUrl("https://search.yahoo.com/search?p=" + toPlainText());
	});
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
void ISQTextEdit::wheelEvent(QWheelEvent *e)
{
	if (Wheel)
	{
		if (e->delta() > NULL)
		{
			zoomIn();
		}
		else
		{
			zoomOut();
		}
	}
	else
	{
		QTextEdit::wheelEvent(e);
	}
}
//-----------------------------------------------------------------------------
void ISQTextEdit::keyPressEvent(QKeyEvent *e)
{
	if (ExecEnter)
	{
		QTextEdit::keyPressEvent(e);

		if (e->modifiers() == Qt::CTRL)
		{
			Wheel = true;
		}
	}
	else
	{
		if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
		{
			e->ignore();
			emit KeyPressEnter();
		}
		else
		{
			QTextEdit::keyPressEvent(e);
		}
	}
}
//-----------------------------------------------------------------------------
void ISQTextEdit::keyReleaseEvent(QKeyEvent *e)
{
	QTextEdit::keyReleaseEvent(e);

	Wheel = false;
}
//-----------------------------------------------------------------------------
void ISQTextEdit::mousePressEvent(QMouseEvent *e)
{
	QTextEdit::mousePressEvent(e);

	if (e->button() == Qt::MiddleButton)
	{
		if (!isReadOnly())
		{
			paste();
		}
	}
}
//-----------------------------------------------------------------------------
