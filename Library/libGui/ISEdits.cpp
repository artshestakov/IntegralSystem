#include "ISEdits.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
#include "ISConstants.h"
#include "ISContextMenu.h"
#include "ISProperty.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISQComboBox::ISQComboBox(QWidget *parent)
	: QComboBox(parent),
	WheelScroll(false)
{
	
}
//-----------------------------------------------------------------------------
ISQComboBox::~ISQComboBox()
{

}
//-----------------------------------------------------------------------------
void ISQComboBox::SetWheelScroll(bool wheel_scroll)
{
	WheelScroll = wheel_scroll;
}
//-----------------------------------------------------------------------------
bool ISQComboBox::GetWheelScrool() const
{
	return WheelScroll;
}
//-----------------------------------------------------------------------------
void ISQComboBox::showPopup()
{
	QComboBox::showPopup();
	emit ShowPopup();
}
//-----------------------------------------------------------------------------
void ISQComboBox::hidePopup()
{
	QComboBox::hidePopup();
	emit HidePopup();
}
//-----------------------------------------------------------------------------
void ISQComboBox::wheelEvent(QWheelEvent *e)
{
	WheelScroll ? QComboBox::wheelEvent(e) : e->ignore();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQDateTimeEdit::ISQDateTimeEdit(QWidget *parent) : QDateTimeEdit(parent)
{
	setStyleSheet(STYLE_SHEET("ISDateTimeEdit"));
	setReadOnly(true);

	ISQLineEdit *LineEdit = new ISQLineEdit(this);
	LineEdit->SetVisibleClear(false);
	setLineEdit(LineEdit);
}
//-----------------------------------------------------------------------------
ISQDateTimeEdit::~ISQDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISQDateTimeEdit::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuDateTime ContextMenu(lineEdit(), lineEdit()->isReadOnly(), lineEdit()->isUndoAvailable(), lineEdit()->isRedoAvailable(), lineEdit()->hasSelectedText(), lineEdit()->echoMode(), lineEdit()->text().isEmpty());
	connect(&ContextMenu, &ISContextMenuDateTime::Delete, lineEdit(), &QLineEdit::del);
	connect(&ContextMenu, &ISContextMenuDateTime::StepUp, this, &ISQDateTimeEdit::stepUp);
	connect(&ContextMenu, &ISContextMenuDateTime::StepDown, this, &ISQDateTimeEdit::stepDown);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQDoubleSpinBox::ISQDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
	setMaximum(MAXIMUM_DOUBLE);

	ISQLineEdit *LineEdit = new ISQLineEdit(this);
	connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISQDoubleSpinBox::ClearClicked);
	setLineEdit(LineEdit);
}
//-----------------------------------------------------------------------------
ISQDoubleSpinBox::~ISQDoubleSpinBox()
{

}
//-----------------------------------------------------------------------------
void ISQDoubleSpinBox::SetVisibleClear(bool Visible)
{
	dynamic_cast<ISQLineEdit*>(lineEdit())->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
QValidator::State ISQDoubleSpinBox::validate(QString &Input, int &Pos) const
{
	if (Input.contains(SYMBOL_POINT))
	{
		Input.replace(SYMBOL_POINT, SYMBOL_COMMA);
	}
	return QDoubleSpinBox::validate(Input, Pos);
}
//-----------------------------------------------------------------------------
void ISQDoubleSpinBox::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuDouble ContextMenu(lineEdit(), lineEdit()->isReadOnly(), lineEdit()->isUndoAvailable(), lineEdit()->isRedoAvailable(), lineEdit()->hasSelectedText(), lineEdit()->echoMode(), lineEdit()->text().isEmpty(), value(), minimum(), maximum());
	connect(&ContextMenu, &ISContextMenuDouble::Delete, lineEdit(), &QLineEdit::del);
	connect(&ContextMenu, &ISContextMenuDouble::StepUp, this, &ISQDoubleSpinBox::stepUp);
	connect(&ContextMenu, &ISContextMenuDouble::StepDown, this, &ISQDoubleSpinBox::stepDown);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQLineEdit::ISQLineEdit(QWidget *parent)
	: QLineEdit(parent),
	MenuCopy(nullptr)
{
	setStyleSheet(STYLE_SHEET("ISLineEdit"));
	setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	setDragEnabled(true);

	if (ISProperty::GetInstance().GetValueBool(IS_PROPERTY_LINE_EDIT_SELECTED_MENU))
	{
		SetMenuSelected(true);
	}

	ActionClear = new QAction(this);
	ActionClear->setToolTip(LANG("Clear.Field"));
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
	setTextMargins(Icon.isNull() ? 0 : 20, 0, 0, 0);
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
		int x = Pixmap.width();

		Painter.drawPixmap(2, 3, Pixmap);
		Painter.setPen(Qt::lightGray);
		Painter.drawLine(x + 2, 3, x + 2, height() - 4);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQSpinBox::ISQSpinBox(QWidget *parent) : QSpinBox(parent)
{
	setMaximum(MAXIMUM_INTEGER);

	ISQLineEdit *LineEdit = new ISQLineEdit(this);
	connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISQSpinBox::ClearClicked);
	setLineEdit(LineEdit);
}
//-----------------------------------------------------------------------------
ISQSpinBox::~ISQSpinBox()
{

}
//-----------------------------------------------------------------------------
void ISQSpinBox::SetCursorPosition(int Position)
{
	lineEdit()->setCursorPosition(Position);
}
//-----------------------------------------------------------------------------
void ISQSpinBox::SetVisibleClear(bool Visible)
{
	dynamic_cast<ISQLineEdit*>(lineEdit())->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISQSpinBox::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuInteger ContextMenu(lineEdit(), lineEdit()->isReadOnly(), lineEdit()->isUndoAvailable(), lineEdit()->isRedoAvailable(), lineEdit()->hasSelectedText(), lineEdit()->echoMode(), lineEdit()->text().isEmpty(), value(), minimum(), maximum());
	connect(&ContextMenu, &ISContextMenuInteger::Delete, lineEdit(), &QLineEdit::del);
	connect(&ContextMenu, &ISContextMenuInteger::StepUp, this, &ISQSpinBox::stepUp);
	connect(&ContextMenu, &ISContextMenuInteger::StepDown, this, &ISQSpinBox::stepDown);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQTextEdit::ISQTextEdit(QWidget *parent)
	: QTextEdit(parent),
	Wheel(false),
	ExecEnter(true)
{
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
		e->delta() > 0 ? zoomIn() : zoomOut();
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