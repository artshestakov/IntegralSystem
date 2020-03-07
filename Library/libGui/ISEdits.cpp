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

	AnimationCalendar = new QPropertyAnimation(CalendarWidget, "windowOpacity", this);
	AnimationCalendar->setDuration(CALENDAR_POPUP_ANIMATION_DURATION);
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
	return text().isEmpty() ? QDate() : QDate::fromString(text(), DATE_FORMAT_V2);
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetDate(const QDate &Date)
{
	if (Date.isValid())
	{
		setText(Date.toString(DATE_FORMAT_V2));
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

	AnimationCalendar->setStartValue(0.0);
	AnimationCalendar->setEndValue(1.0);

	CalendarWidget->move(Point);
	CalendarWidget->setSelectedDate(GetDate());
	CalendarWidget->setWindowOpacity(0.0);
	CalendarWidget->show();
	CalendarWidget->setFocus();
	AnimationCalendar->start();
}
//-----------------------------------------------------------------------------
void ISQDateEdit::HideCalendar()
{
	AnimationCalendar->setStartValue(1.0);
	AnimationCalendar->setEndValue(0.0);

	setFocus();
	AnimationCalendar->start();
	QTimer::singleShot(CALENDAR_POPUP_ANIMATION_DURATION, CalendarWidget, &ISCalendarPopup::hide);
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

	ComboBoxHour = new ISQComboBox(this);
	ComboBoxHour->SetWheelScroll(true);
	ComboBoxHour->setCursor(Qt::PointingHandCursor);
	ComboBoxHour->addItem("00", 0);
	ComboBoxHour->addItem("01", 1);
	ComboBoxHour->addItem("02", 2);
	ComboBoxHour->addItem("03", 3);
	ComboBoxHour->addItem("04", 4);
	ComboBoxHour->addItem("05", 5);
	ComboBoxHour->addItem("06", 6);
	ComboBoxHour->addItem("07", 7);
	ComboBoxHour->addItem("08", 8);
	ComboBoxHour->addItem("09", 9);
	ComboBoxHour->addItem("10", 10);
	ComboBoxHour->addItem("11", 11);
	ComboBoxHour->addItem("12", 12);
	ComboBoxHour->addItem("13", 13);
	ComboBoxHour->addItem("14", 14);
	ComboBoxHour->addItem("15", 15);
	ComboBoxHour->addItem("16", 16);
	ComboBoxHour->addItem("17", 17);
	ComboBoxHour->addItem("18", 18);
	ComboBoxHour->addItem("19", 19);
	ComboBoxHour->addItem("20", 20);
	ComboBoxHour->addItem("21", 21);
	ComboBoxHour->addItem("22", 22);
	ComboBoxHour->addItem("23", 23);
	connect(ComboBoxHour, static_cast<void(ISQComboBox::*)(int)>(&ISQComboBox::currentIndexChanged), this, &ISQTimeEdit::TimeChanged);
	Layout->addWidget(ComboBoxHour);

	Layout->addWidget(new QLabel(":", this));

	ComboBoxMinute = new ISQComboBox(this);
	ComboBoxMinute->SetWheelScroll(true);
	ComboBoxMinute->setCursor(Qt::PointingHandCursor);
	ComboBoxMinute->addItem("00", 0);
	ComboBoxMinute->addItem("01", 1);
	ComboBoxMinute->addItem("02", 2);
	ComboBoxMinute->addItem("03", 3);
	ComboBoxMinute->addItem("04", 4);
	ComboBoxMinute->addItem("05", 5);
	ComboBoxMinute->addItem("06", 6);
	ComboBoxMinute->addItem("07", 7);
	ComboBoxMinute->addItem("08", 8);
	ComboBoxMinute->addItem("09", 9);
	ComboBoxMinute->addItem("10", 10);
	ComboBoxMinute->addItem("11", 11);
	ComboBoxMinute->addItem("12", 12);
	ComboBoxMinute->addItem("13", 13);
	ComboBoxMinute->addItem("14", 14);
	ComboBoxMinute->addItem("15", 15);
	ComboBoxMinute->addItem("16", 16);
	ComboBoxMinute->addItem("17", 17);
	ComboBoxMinute->addItem("18", 18);
	ComboBoxMinute->addItem("19", 19);
	ComboBoxMinute->addItem("20", 20);
	ComboBoxMinute->addItem("21", 21);
	ComboBoxMinute->addItem("22", 22);
	ComboBoxMinute->addItem("23", 23);
	ComboBoxMinute->addItem("24", 24);
	ComboBoxMinute->addItem("25", 25);
	ComboBoxMinute->addItem("26", 26);
	ComboBoxMinute->addItem("27", 27);
	ComboBoxMinute->addItem("28", 28);
	ComboBoxMinute->addItem("29", 29);
	ComboBoxMinute->addItem("30", 30);
	ComboBoxMinute->addItem("31", 31);
	ComboBoxMinute->addItem("32", 32);
	ComboBoxMinute->addItem("33", 33);
	ComboBoxMinute->addItem("34", 34);
	ComboBoxMinute->addItem("35", 35);
	ComboBoxMinute->addItem("36", 36);
	ComboBoxMinute->addItem("37", 37);
	ComboBoxMinute->addItem("38", 38);
	ComboBoxMinute->addItem("39", 39);
	ComboBoxMinute->addItem("40", 40);
	ComboBoxMinute->addItem("41", 41);
	ComboBoxMinute->addItem("42", 42);
	ComboBoxMinute->addItem("43", 43);
	ComboBoxMinute->addItem("44", 44);
	ComboBoxMinute->addItem("45", 45);
	ComboBoxMinute->addItem("46", 46);
	ComboBoxMinute->addItem("47", 47);
	ComboBoxMinute->addItem("48", 48);
	ComboBoxMinute->addItem("49", 49);
	ComboBoxMinute->addItem("50", 50);
	ComboBoxMinute->addItem("51", 51);
	ComboBoxMinute->addItem("52", 52);
	ComboBoxMinute->addItem("53", 53);
	ComboBoxMinute->addItem("54", 54);
	ComboBoxMinute->addItem("55", 55);
	ComboBoxMinute->addItem("56", 56);
	ComboBoxMinute->addItem("57", 57);
	ComboBoxMinute->addItem("58", 58);
	ComboBoxMinute->addItem("59", 59);
	connect(ComboBoxMinute, static_cast<void(ISQComboBox::*)(int)>(&ISQComboBox::currentIndexChanged), this, &ISQTimeEdit::TimeChanged);
	Layout->addWidget(ComboBoxMinute);

	Layout->addWidget(new QLabel(":", this));

	ComboBoxSecond = new ISQComboBox(this);
	ComboBoxSecond->SetWheelScroll(true);
	ComboBoxSecond->setCursor(Qt::PointingHandCursor);
	ComboBoxSecond->addItem("00", 0);
	ComboBoxSecond->addItem("01", 1);
	ComboBoxSecond->addItem("02", 2);
	ComboBoxSecond->addItem("03", 3);
	ComboBoxSecond->addItem("04", 4);
	ComboBoxSecond->addItem("05", 5);
	ComboBoxSecond->addItem("06", 6);
	ComboBoxSecond->addItem("07", 7);
	ComboBoxSecond->addItem("08", 8);
	ComboBoxSecond->addItem("09", 9);
	ComboBoxSecond->addItem("10", 10);
	ComboBoxSecond->addItem("11", 11);
	ComboBoxSecond->addItem("12", 12);
	ComboBoxSecond->addItem("13", 13);
	ComboBoxSecond->addItem("14", 14);
	ComboBoxSecond->addItem("15", 15);
	ComboBoxSecond->addItem("16", 16);
	ComboBoxSecond->addItem("17", 17);
	ComboBoxSecond->addItem("18", 18);
	ComboBoxSecond->addItem("19", 19);
	ComboBoxSecond->addItem("20", 20);
	ComboBoxSecond->addItem("21", 21);
	ComboBoxSecond->addItem("22", 22);
	ComboBoxSecond->addItem("23", 23);
	ComboBoxSecond->addItem("24", 24);
	ComboBoxSecond->addItem("25", 25);
	ComboBoxSecond->addItem("26", 26);
	ComboBoxSecond->addItem("27", 27);
	ComboBoxSecond->addItem("28", 28);
	ComboBoxSecond->addItem("29", 29);
	ComboBoxSecond->addItem("30", 30);
	ComboBoxSecond->addItem("31", 31);
	ComboBoxSecond->addItem("32", 32);
	ComboBoxSecond->addItem("33", 33);
	ComboBoxSecond->addItem("34", 34);
	ComboBoxSecond->addItem("35", 35);
	ComboBoxSecond->addItem("36", 36);
	ComboBoxSecond->addItem("37", 37);
	ComboBoxSecond->addItem("38", 38);
	ComboBoxSecond->addItem("39", 39);
	ComboBoxSecond->addItem("40", 40);
	ComboBoxSecond->addItem("41", 41);
	ComboBoxSecond->addItem("42", 42);
	ComboBoxSecond->addItem("43", 43);
	ComboBoxSecond->addItem("44", 44);
	ComboBoxSecond->addItem("45", 45);
	ComboBoxSecond->addItem("46", 46);
	ComboBoxSecond->addItem("47", 47);
	ComboBoxSecond->addItem("48", 48);
	ComboBoxSecond->addItem("49", 49);
	ComboBoxSecond->addItem("50", 50);
	ComboBoxSecond->addItem("51", 51);
	ComboBoxSecond->addItem("52", 52);
	ComboBoxSecond->addItem("53", 53);
	ComboBoxSecond->addItem("54", 54);
	ComboBoxSecond->addItem("55", 55);
	ComboBoxSecond->addItem("56", 56);
	ComboBoxSecond->addItem("57", 57);
	ComboBoxSecond->addItem("58", 58);
	ComboBoxSecond->addItem("59", 59);
	connect(ComboBoxSecond, static_cast<void(ISQComboBox::*)(int)>(&ISQComboBox::currentIndexChanged), this, &ISQTimeEdit::TimeChanged);
	Layout->addWidget(ComboBoxSecond);

	Clear();
}
//-----------------------------------------------------------------------------
ISQTimeEdit::~ISQTimeEdit()
{

}
//-----------------------------------------------------------------------------
QTime ISQTimeEdit::GetTime() const
{
	return QTime(ComboBoxHour->currentData().toInt(), ComboBoxMinute->currentData().toInt(), ComboBoxSecond->currentData().toInt());
}
//-----------------------------------------------------------------------------
void ISQTimeEdit::SetTime(const QTime &Time)
{
	if (Time.isValid() && !Time.isNull())
	{
		ComboBoxHour->setCurrentIndex(Time.hour());
		ComboBoxMinute->setCurrentIndex(Time.minute());
		ComboBoxSecond->setCurrentIndex(Time.second());
	}
}
//-----------------------------------------------------------------------------
void ISQTimeEdit::Clear()
{
	ComboBoxHour->setCurrentIndex(0);
	ComboBoxMinute->setCurrentIndex(0);
	ComboBoxSecond->setCurrentIndex(0);
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