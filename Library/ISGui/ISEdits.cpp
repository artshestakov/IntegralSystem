#include "ISEdits.h"
#include "ISStyleSheet.h"
#include "ISGui.h"
#include "ISConstants.h"
#include "ISContextMenu.h"
#include "ISProperty.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISPhoneNumberParser.h"
#include "ISDefinesGui.h"
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
ISQLineEdit::ISQLineEdit(QWidget *parent)
	: QLineEdit(parent),
	MenuCopy(nullptr)
{
	setStyleSheet(STYLE_SHEET("ISLineEdit"));
	setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	setDragEnabled(true);
	SetMenuSelected(ISProperty::Instance().GetValue(PROPERTY_LINE_EDIT_SELECTED_MENU).toBool());

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
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
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
void ISQLineEdit::paintEvent(QPaintEvent *e)
{
	QLineEdit::paintEvent(e);
	if (!Icon.isNull())
	{
		QPixmap Pixmap = Icon.pixmap(height() - 6, height() - 6);
		int x = Pixmap.width();

		QPainter Painter(this);
		Painter.drawPixmap(2, 3, Pixmap);
		Painter.setPen(Qt::lightGray);
		Painter.drawLine(x + 2, 3, x + 2, height() - 4);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQDateEdit::ISQDateEdit(QWidget *parent)
	: ISQLineEdit(parent),
	IsShow(false)
{
	setReadOnly(true);
	SetVisibleClear(false);

	ButtonCalendar = new ISServiceButton(this);
	ButtonCalendar->setToolTip(LANG("ShowCalendar") );
	ButtonCalendar->setIcon(BUFFER_ICONS("Calendar"));
	ButtonCalendar->setFocusPolicy(Qt::NoFocus);
	ButtonCalendar->setFlat(true);
	connect(ButtonCalendar, &ISServiceButton::clicked, this, &ISQDateEdit::ShowCalendar);

	CalendarWidget = new ISCalendarPopup(this);
	CalendarWidget->setVisible(false);
	connect(CalendarWidget, &ISCalendarWidget::clicked, this, &ISQDateEdit::SetDate);
	connect(CalendarWidget, &ISCalendarWidget::Hide, this, &ISQDateEdit::HideCalendar);
}
//-----------------------------------------------------------------------------
ISQDateEdit::~ISQDateEdit()
{

}
//-----------------------------------------------------------------------------
void ISQDateEdit::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuBase ContextMenu(this, isReadOnly(), isUndoAvailable(), isRedoAvailable(), hasSelectedText(), echoMode(), text().isEmpty());
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
QDate ISQDateEdit::GetDate() const
{
	return text().isEmpty() ? QDate() : QDate::fromString(text(), FORMAT_DATE_V2);
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetDate(const QDate &Date)
{
	if (Date.isValid())
	{
		setText(Date.toString(FORMAT_DATE_V2));
		emit DateChanged();
		HideCalendar();
	}
}
//-----------------------------------------------------------------------------
void ISQDateEdit::Clear()
{
	clear();
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetReadOnly(bool ReadOnly)
{
	setAlignment(ReadOnly ? Qt::AlignHCenter : Qt::AlignLeft);
	ButtonCalendar->setVisible(!ReadOnly);
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetRange(const QDate &Minimum, const QDate &Maximum)
{
	CalendarWidget->setDateRange(Minimum, Maximum);
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetMinimumDate(const QDate &Date)
{
	CalendarWidget->setMinimumDate(Date);
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetMaximumDate(const QDate &Date)
{
	CalendarWidget->setMaximumDate(Date);
}
//-----------------------------------------------------------------------------
void ISQDateEdit::showEvent(QShowEvent *ShowEvent)
{
	ISQLineEdit::showEvent(ShowEvent);
	if (!IsShow)
	{
		ButtonCalendar->adjustSize();
		ButtonCalendar->setFixedSize(ButtonCalendar->width() - 2, ButtonCalendar->height() - 2);
		IsShow = true;
	}
}
//-----------------------------------------------------------------------------
void ISQDateEdit::resizeEvent(QResizeEvent *ResizeEvent)
{
	ISQLineEdit::resizeEvent(ResizeEvent);
	ButtonCalendar->move(QPoint(width() - ButtonCalendar->width() - 2, 2));
}
//-----------------------------------------------------------------------------
void ISQDateEdit::ShowCalendar()
{
	QPoint Point = mapToGlobal(QPoint());
	Point.setY(Point.y() + height());

	CalendarWidget->move(Point);
	CalendarWidget->setSelectedDate(GetDate());
	CalendarWidget->show();
	CalendarWidget->setFocus();
}
//-----------------------------------------------------------------------------
void ISQDateEdit::HideCalendar()
{
	CalendarWidget->hide();
	setFocus();
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SelectionChanged()
{
	SetDate(CalendarWidget->selectedDate());
	CalendarWidget->hide();
	emit DateChanged();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISQTimeEdit::ISQTimeEdit(QWidget *parent) : QWidget(parent)
{
	setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	setSizePolicy(QSizePolicy::Maximum, sizePolicy().verticalPolicy());

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(Layout->contentsMargins().left(), 0, Layout->contentsMargins().right(), 0);
	setLayout(Layout);

	EditHour = new ISQLineEdit(this);
	EditHour->setAlignment(Qt::AlignHCenter);
	EditHour->setValidator(new QIntValidator(0, 23, EditHour));
	EditHour->setMaximumWidth(30);
	EditHour->SetVisibleClear(false);
	EditHour->setMaxLength(2);
	connect(EditHour, &ISQLineEdit::textChanged, this, &ISQTimeEdit::TimeChanged);
	Layout->addWidget(EditHour);

	Layout->addWidget(new QLabel(":", this));

	EditMinute = new ISQLineEdit(this);
	EditMinute->setAlignment(Qt::AlignHCenter);
	EditMinute->setValidator(new QIntValidator(0, 59, EditMinute));
	EditMinute->setMaximumWidth(30);
	EditMinute->SetVisibleClear(false);
	EditMinute->setMaxLength(2);
	connect(EditMinute, &ISQLineEdit::textChanged, this, &ISQTimeEdit::TimeChanged);
	Layout->addWidget(EditMinute);

	Layout->addWidget(new QLabel(":", this));

	EditSecond = new ISQLineEdit(this);
	EditSecond->setValidator(new QIntValidator(0, 59, EditSecond));
	EditSecond->setAlignment(Qt::AlignHCenter);
	EditSecond->setMaximumWidth(30);
	EditSecond->SetVisibleClear(false);
	EditSecond->setMaxLength(2);
	connect(EditSecond, &ISQLineEdit::textChanged, this, &ISQTimeEdit::TimeChanged);
	Layout->addWidget(EditSecond);

	Clear();
}
//-----------------------------------------------------------------------------
ISQTimeEdit::~ISQTimeEdit()
{

}
//-----------------------------------------------------------------------------
QTime ISQTimeEdit::GetTime() const
{
	return QTime(EditHour->text().toInt(), EditMinute->text().toInt(), EditSecond->text().toInt());
}
//-----------------------------------------------------------------------------
void ISQTimeEdit::SetTime(const QTime &Time)
{
	if (Time.isValid() && !Time.isNull())
	{
		char Char[3];
		sprintf(Char, "%02d", Time.hour());
		EditHour->setText(Char);

		sprintf(Char, "%02d", Time.minute());
		EditMinute->setText(Char);

		sprintf(Char, "%02d", Time.second());
		EditSecond->setText(Char);
	}
}
//-----------------------------------------------------------------------------
void ISQTimeEdit::Clear()
{
	EditHour->clear();
	EditMinute->clear();
	EditSecond->clear();
}
//-----------------------------------------------------------------------------
void ISQTimeEdit::SetReadOnly(bool ReadOnly)
{
	EditHour->setReadOnly(ReadOnly);
	EditMinute->setReadOnly(ReadOnly);
	EditSecond->setReadOnly(ReadOnly);
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISPhoneLineEdit::ISPhoneLineEdit(QWidget *parent)
	: QLineEdit(parent),
	LastPosition(0)
{
	setInputMask("(000) 000-00-00;_");
	setTextMargins(45, 0, 0, 0);
	setStyleSheet(STYLE_SHEET("ISLineEdit"));
	setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	connect(this, &ISPhoneLineEdit::cursorPositionChanged, this, &ISPhoneLineEdit::CursorPositionChanged);

	ActionCut = ISControls::GetActionContextCut(this);
	connect(ActionCut, &QAction::triggered, this, &ISPhoneLineEdit::CutClicked);

	ActionCopy = ISControls::GetActionContextCopy(this);
	connect(ActionCopy, &QAction::triggered, this, &ISPhoneLineEdit::CopyClicked);

	ActionPaste = ISControls::GetActionContextPaste(this);
	connect(ActionPaste, &QAction::triggered, this, &ISPhoneLineEdit::PasteClicked);

	ActionClear = new QAction(this);
	ActionClear->setToolTip(LANG("Clear.Field"));
	ActionClear->setIcon(BUFFER_ICONS("ButtonClear"));
	connect(ActionClear, &QAction::triggered, this, &ISPhoneLineEdit::ClearClicked);
	addAction(ActionClear, QLineEdit::TrailingPosition);
}
//-----------------------------------------------------------------------------
ISPhoneLineEdit::~ISPhoneLineEdit()
{

}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::SetIcon(ISNamespace::PhoneIconType IconType)
{
	switch (IconType)
	{
	case ISNamespace::PIT_Standart: Icon = BUFFER_ICONS("Phone.Correcting.Standart").pixmap(ISDefines::Gui::SIZE_16_16); break;
	case ISNamespace::PIT_TRUE: Icon = BUFFER_ICONS("Phone.Correcting.True").pixmap(ISDefines::Gui::SIZE_16_16); break;
	case ISNamespace::PIT_FALSE: Icon = BUFFER_ICONS("Phone.Correcting.False").pixmap(ISDefines::Gui::SIZE_16_16); break;
	}
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::ResetLastPosition()
{
	LastPosition = 0;
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::CutClicked()
{
	CopyClicked();
	clear();
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::CopyClicked()
{
	QApplication::clipboard()->setText("+7" + ISPhoneNumberParser::RemoveNotDigits(text()));
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::PasteClicked()
{
	emit Pasted(QApplication::clipboard()->text());
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::CursorPositionChanged(int OldPos, int NewPos)
{
	Q_UNUSED(OldPos);
	LastPosition = NewPos;
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::paintEvent(QPaintEvent *e)
{
	QLineEdit::paintEvent(e);

	QPainter Painter(this);
	QPixmap Pixmap = Icon.pixmap(height() - 6, height() - 6);

	Painter.drawPixmap(2, 3, Pixmap);
	Painter.setPen(Qt::lightGray);
	Painter.drawLine(Pixmap.width() + 3, 3, Pixmap.width() + 3, height() - 4);
	Painter.setPen(Qt::black);
	Painter.drawText(25, 17, "+7");
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::focusInEvent(QFocusEvent *e)
{
	QLineEdit::focusInEvent(e);

	int TextLenght = ISPhoneNumberParser::RemoveNotDigits(text()).length();
	if (LastPosition > 0 && LastPosition <= TextLenght) //Если номер телефона не закончили вводить
	{
		setCursorPosition(LastPosition);
	}
	else if (LastPosition == 0 || TextLenght == 10) //Номер телефона введен корректно или его нет вообще
	{
		QTimer::singleShot(10, this, &ISPhoneLineEdit::selectAll);
	}
	else
	{
		QTimer::singleShot(10, this, &ISPhoneLineEdit::selectAll);
	}
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::keyPressEvent(QKeyEvent *e)
{
	if (e->modifiers() == Qt::CTRL && e->key() == Qt::Key_V || e->modifiers() == Qt::SHIFT && e->key() == Qt::Key_Insert) //Если происходит вставка номера
	{
		e->ignore();
		PasteClicked();
	}
	else //Обработка нажатия остальных кнопок
	{
		QLineEdit::keyPressEvent(e);
	}
}
//-----------------------------------------------------------------------------
void ISPhoneLineEdit::contextMenuEvent(QContextMenuEvent *e)
{
	QString PhoneString = ISPhoneNumberParser::RemoveNotDigits(text());
	if (PhoneString.length() == 0)
	{
		ActionCut->setEnabled(false);
		ActionCopy->setEnabled(false);
	}
	else if (PhoneString.length() == 10)
	{
		ActionCut->setEnabled(true);
		ActionCopy->setEnabled(true);
	}
	else
	{
		ActionCut->setEnabled(false);
		ActionCopy->setEnabled(false);
	}

	ISGui::SetWaitGlobalCursor(true);
	QMenu ContextMenu;
	ContextMenu.addAction(ActionCut);
	ContextMenu.addAction(ActionCopy);
	ContextMenu.addAction(ActionPaste);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
