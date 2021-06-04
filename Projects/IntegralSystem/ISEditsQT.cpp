#include "ISEditsQT.h"
#include "ISGui.h"
#include "ISConstantsOld.h"
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
    : QLineEdit(parent)
{
    setStyleSheet(BUFFER_STYLE_SHEET("ISLineEdit"));
    setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    setDragEnabled(true);

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
void ISQLineEdit::SetEnabled(bool Enabled)
{
    setEnabled(Enabled);
    if (!PixmapCurrent.isNull())
    {
        if (Enabled)
        {
            PixmapCurrent = PixmapEnabled;
        }
        else
        {
            QImage Image = PixmapCurrent.toImage();
            for (int y = 0, Height = Image.height(); y < Height; ++y)
            {
                for (int x = 0, Width = Image.width(); x < Width; ++x)
                {
                    QRgb Rgb = Image.pixel(x, y);
                    if (Rgb > 0)
                    {
                        Image.setPixelColor(x, y, Qt::gray);
                    }
                }
            }
            PixmapCurrent = QPixmap::fromImage(Image);
        }
    }
}
//-----------------------------------------------------------------------------
void ISQLineEdit::SetIcon(const QIcon &icon)
{
    PixmapCurrent = icon.pixmap(height() - 3, width() - 3);
    PixmapEnabled = PixmapCurrent;
    setTextMargins(PixmapCurrent.isNull() ? 0 : height() + 2, 0, 0, 0);
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
void ISQLineEdit::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);
    if (!PixmapCurrent.isNull())
    {
        int x = height();
        QPainter Painter(this);
        Painter.drawPixmap(1, 1, PixmapCurrent);
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

    ButtonCalendar = new ISServiceButton(BUFFER_ICONS("Calendar"), LANG("ShowCalendar"), this);
    ButtonCalendar->setFlat(true);
    ButtonCalendar->setFocusPolicy(Qt::NoFocus);
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
    disconnect(CalendarWidget, &ISCalendarWidget::clicked, this, &ISQDateEdit::SetDate);
    CalendarWidget->setSelectedDate(QDate::currentDate());
    connect(CalendarWidget, &ISCalendarWidget::clicked, this, &ISQDateEdit::SetDate);
    clear();
}
//-----------------------------------------------------------------------------
void ISQDateEdit::SetEnabled(bool Enabled)
{
    ISQLineEdit::SetEnabled(Enabled);
    ButtonCalendar->setEnabled(Enabled);
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
    bool HourOk = true, MinuteOk = true, SecondOk = true;
    int Hour = EditHour->text().toInt(&HourOk),
        Minute = EditMinute->text().toInt(&MinuteOk),
        Second = EditSecond->text().toInt(&SecondOk);
    return HourOk && MinuteOk && SecondOk ? QTime(Hour, Minute, Second) : QTime();
}
//-----------------------------------------------------------------------------
void ISQTimeEdit::SetTime(const QTime &Time)
{
    if (Time.isValid() && !Time.isNull())
    {
        char Char[3] = { 0 };
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
    setStyleSheet(BUFFER_STYLE_SHEET("ISTextEdit"));
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
