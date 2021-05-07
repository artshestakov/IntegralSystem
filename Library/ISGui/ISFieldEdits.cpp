#include "ISFieldEdits.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISDialogsCommon.h"
#include "ISGui.h"
#include "ISPopupMessage.h"
#include "ISDefinesGui.h"
#include "ISSettingsDatabase.h"
#include "ISSettings.h"
#include "ISUserRoleEntity.h"
#include "ISControls.h"
#include "ISMetaData.h"
#include "ISDelegates.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISCheckEdit::ISCheckEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);

    CheckBox = new QCheckBox(this);
    CheckBox->setChecked(false);
    CheckBox->setTristate(false);
    CheckBox->setCursor(CURSOR_POINTING_HAND);
    connect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::ValueChanged);
    AddWidgetEdit(CheckBox, this);
}
//-----------------------------------------------------------------------------
ISCheckEdit::~ISCheckEdit()
{

}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetValue(const QVariant &value)
{
    CheckBox->setChecked(value.toBool());
}
//-----------------------------------------------------------------------------
QVariant ISCheckEdit::GetValue() const
{
    return CheckBox->isChecked();
}
//-----------------------------------------------------------------------------
void ISCheckEdit::Clear()
{
    CheckBox->setChecked(false);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetReadOnly(bool read_only)
{
    CheckBox->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetText(const QString &Text)
{
    CheckBox->setText(Text);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetCheckableStrikeOut(bool StrikeOut)
{
    CheckableStrikeOut(); //Вызываем на всякий случай, вдруг сейчас CheckEdit уже активен
    StrikeOut ?
        connect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::CheckableStrikeOut) :
        disconnect(CheckBox, &QCheckBox::stateChanged, this, &ISCheckEdit::CheckableStrikeOut);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetIcon(const QIcon &Icon)
{
    CheckBox->setIcon(Icon);
}
//-----------------------------------------------------------------------------
void ISCheckEdit::SetIconSize(const QSize &Size)
{
    CheckBox->setIconSize(Size);
}
//-----------------------------------------------------------------------------
QCheckBox* ISCheckEdit::GetCheckBox()
{
    return CheckBox;
}
//-----------------------------------------------------------------------------
void ISCheckEdit::CheckableStrikeOut()
{
    QFont Font = CheckBox->font();
    Font.setStrikeOut(CheckBox->isChecked());
    CheckBox->setFont(Font);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISLineEdit::ISLineEdit(QWidget *parent)
    : ISFieldEditBase(parent),
    Completer(nullptr)
{
    LineEdit = new ISQLineEdit(this);
    connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISLineEdit::Clear);
    connect(LineEdit, &ISQLineEdit::returnPressed, this, &ISLineEdit::EnterClicked);
    connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::TextChanged);
    AddWidgetEdit(LineEdit, this);
}
//-----------------------------------------------------------------------------
ISLineEdit::~ISLineEdit()
{

}
//-----------------------------------------------------------------------------
void ISLineEdit::SetValue(const QVariant &value)
{
    LineEdit->setText(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISLineEdit::GetValue() const
{
    return LineEdit->text().isEmpty() ? QVariant() : LineEdit->text();
}
//-----------------------------------------------------------------------------
void ISLineEdit::Clear()
{
    LineEdit->clear();
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetRegExp(const QString &RegExp)
{
    SetValidator(new QRegExpValidator(QRegExp(RegExp), this));
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetColorText(const QColor &Color)
{
    QPalette Palette = LineEdit->palette();
    Palette.setColor(QPalette::Text, Color);
    LineEdit->setPalette(Palette);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetVisibleClear(bool Visible)
{
    LineEdit->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetValidator(QValidator *Validator)
{
    LineEdit->setValidator(Validator);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetEchoMode(QLineEdit::EchoMode EchoMode)
{
    LineEdit->setEchoMode(EchoMode);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetCompleter(QCompleter *completer)
{
    LineEdit->setCompleter(completer);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetTextAlignment(Qt::Alignment Alignment)
{
    LineEdit->setAlignment(Alignment);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetUppercase(bool uppercase)
{
    uppercase ?
        connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText) :
        disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetLowercase(bool lowercase)
{
    lowercase ?
        connect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText) :
        disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
void ISLineEdit::ResetFontcase()
{
    disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnUpperText);
    disconnect(LineEdit, &ISQLineEdit::textChanged, this, &ISLineEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
void ISLineEdit::AddAction(QAction *Action, QLineEdit::ActionPosition Position)
{
    LineEdit->AddAction(Action, Position);
}
//-----------------------------------------------------------------------------
void ISLineEdit::CreateCompleter(const QStringList &StringList)
{
    POINTER_DELETE(Completer);
    Completer = new QCompleter(StringList, this);
    Completer->setMaxVisibleItems(20);
    Completer->setFilterMode(Qt::MatchContains);
    Completer->setCaseSensitivity(Qt::CaseInsensitive);
    Completer->setCompletionMode(QCompleter::PopupCompletion);
    SetCompleter(Completer);
}
//-----------------------------------------------------------------------------
QStringList ISLineEdit::GetCompleterList() const
{
    QStringList StringList;
    for (int i = 0; i < Completer->model()->rowCount(); ++i)
    {
        StringList.append(Completer->model()->data(Completer->model()->index(i, 0)).toString());
    }
    return StringList;
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetReadOnly(bool read_only)
{
    ISFieldEditBase::SetReadOnly(read_only);
    SetVisibleClear(!read_only);
    LineEdit->setReadOnly(read_only);
    LineEdit->SetVisibleClear(!read_only);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetEnabled(bool Enabled)
{
    ISFieldEditBase::SetEnabled(Enabled);
    LineEdit->SetEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetPlaceholderText(const QString &placeholder_text)
{
    LineEdit->setPlaceholderText(placeholder_text);
}
//-----------------------------------------------------------------------------
void ISLineEdit::OnUpperText(const QString &Text)
{
    LineEdit->setText(Text.toUpper());
    LineEdit->setCursorPosition(LineEdit->cursorPosition());
}
//-----------------------------------------------------------------------------
void ISLineEdit::OnLowerText(const QString &Text)
{
    LineEdit->setText(Text.toLower());
    LineEdit->setCursorPosition(LineEdit->cursorPosition());
}
//-----------------------------------------------------------------------------
void ISLineEdit::TextChanged(const QString &Text)
{
    Q_UNUSED(Text);
    emit ValueChanged();
}
//-----------------------------------------------------------------------------
ISQLineEdit* ISLineEdit::GetLineEdit()
{
    return LineEdit;
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetInputMask(const QString &InputMask)
{
    LineEdit->setInputMask(InputMask);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetFocusPolicy(Qt::FocusPolicy focus_policy)
{
    LineEdit->setFocusPolicy(focus_policy);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetTextMargins(const QMargins &Margins)
{
    LineEdit->setTextMargins(Margins);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetTextMargins(int Left, int Top, int Right, int Bottom)
{
    LineEdit->setTextMargins(Left, Top, Right, Bottom);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetMaxLength(int Length)
{
    LineEdit->setMaxLength(Length);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SetIcon(const QIcon &Icon)
{
    LineEdit->SetIcon(Icon);
}
//-----------------------------------------------------------------------------
void ISLineEdit::SelectAll()
{
    QTimer::singleShot(10, GetLineEdit(), &ISQLineEdit::selectAll);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboEdit::ISComboEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    ISQLineEdit *LineEdit = new ISQLineEdit(this);
    LineEdit->SetVisibleClear(false);
    LineEdit->setStyleSheet(QString());
    LineEdit->setFixedHeight(20);

    ComboBox = new ISQComboBox(this);
    ComboBox->setLineEdit(LineEdit);
    ComboBox->setItemDelegate(new ISPopupDelegate(ComboBox));
    ComboBox->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    ComboBox->setIconSize(ISDefines::Gui::SIZE_22_22);
    connect(ComboBox, static_cast<void(ISQComboBox::*)(int Index)>(&ISQComboBox::currentIndexChanged), this, &ISComboEdit::ValueChanged);
    AddWidgetEdit(ComboBox, this);
}
//-----------------------------------------------------------------------------
ISComboEdit::~ISComboEdit()
{

}
//-----------------------------------------------------------------------------
void ISComboEdit::SetValue(const QVariant &value)
{
    for (int i = 0; i < ComboBox->count(); ++i)
    {
        if (value == ComboBox->itemData(i))
        {
            ComboBox->setCurrentIndex(i);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
QVariant ISComboEdit::GetValue() const
{
    QVariant UserData = ComboBox->itemData(ComboBox->currentIndex());
    return UserData.isNull() ? QVariant() : UserData;
}
//-----------------------------------------------------------------------------
void ISComboEdit::Clear()
{
    ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
QString ISComboEdit::GetCurrentText() const
{
    return ComboBox->currentText();
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetEditable(bool Editable)
{
    ComboBox->setEditable(Editable);
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetCurrentItem(int Index)
{
    ComboBox->setCurrentIndex(Index);
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetCurrentText(const QString &Text)
{
    ComboBox->setCurrentText(Text);
}
//-----------------------------------------------------------------------------
void ISComboEdit::InsertItem(int Index, const QString &Text, const QVariant &UserData)
{
    ComboBox->insertItem(Index, Text, UserData);
}
//-----------------------------------------------------------------------------
void ISComboEdit::AddItem(const QIcon &IconItem, const QString &Text, const QVariant &UserData)
{
    ComboBox->addItem(IconItem, Text, UserData);
}
//-----------------------------------------------------------------------------
void ISComboEdit::AddItem(const QString &Text, const QVariant &UserData)
{
    ComboBox->addItem(Text, UserData);
}
//-----------------------------------------------------------------------------
void ISComboEdit::AddItem(const QString &Text)
{
    ComboBox->addItem(Text);
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetIcon(int Index, const QIcon &Icon)
{
    ComboBox->setItemIcon(Index, Icon);
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetWheelScroll(bool WheelScroll)
{
    ComboBox->SetWheelScroll(WheelScroll);
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetDuplicatesEnabled(bool Enabled)
{
    ComboBox->setDuplicatesEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISComboEdit::HideFirstItem()
{
    ComboBox->model()->removeRow(0);
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetReadOnly(bool read_only)
{
    ComboBox->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
void ISComboEdit::CurrentIndexChanged(int Index)
{
    Q_UNUSED(Index);
    emit ValueChanged();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISRadioEdit::ISRadioEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    setSizePolicy(QSizePolicy::Maximum, sizePolicy().verticalPolicy());

    LayoutPanel = new QHBoxLayout();
    LayoutPanel->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
    LayoutPanel->addStretch();

    WidgetPanel = new QWidget(this);
    WidgetPanel->setLayout(LayoutPanel);
    AddWidgetEdit(WidgetPanel, this);

    ButtonGroup = new QButtonGroup(this);
    connect(ButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ISRadioEdit::ValueChanged);
}
//-----------------------------------------------------------------------------
ISRadioEdit::~ISRadioEdit()
{

}
//-----------------------------------------------------------------------------
void ISRadioEdit::SetValue(const QVariant &value)
{
    ButtonGroup->button(MapButtons.key(value))->setChecked(true);
}
//-----------------------------------------------------------------------------
QVariant ISRadioEdit::GetValue() const
{
    QVariant Value = MapButtons.value(ButtonGroup->checkedId());
    if (Value.isValid())
    {
        return Value;
    }

    return QVariant();
}
//-----------------------------------------------------------------------------
void ISRadioEdit::Clear()
{
    ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISRadioEdit::AddButton(QRadioButton *RadioButton, const QVariant &Value)
{
    RadioButton->setParent(WidgetPanel);
    RadioButton->setCursor(CURSOR_POINTING_HAND);

    int ButtonID = ButtonGroup->buttons().count();
    ButtonGroup->addButton(RadioButton, ButtonID);
    MapButtons.insert(ButtonID, Value);

    LayoutPanel->insertWidget(ButtonID, RadioButton);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTextEdit::ISTextEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Minimum);
    CreateButtonClear();

    TextEdit = new ISQTextEdit(this);
    TextEdit->setTabChangesFocus(true);
    TextEdit->setAcceptRichText(false);
    connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::ValueChanged);
    connect(TextEdit, &ISQTextEdit::KeyPressEnter, this, &ISTextEdit::KeyPressEnter);
    AddWidgetEdit(TextEdit, this);
}
//-----------------------------------------------------------------------------
ISTextEdit::~ISTextEdit()
{

}
//-----------------------------------------------------------------------------
void ISTextEdit::SetValue(const QVariant &value)
{
    TextEdit->setText(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISTextEdit::GetValue() const
{
    if (TextEdit->toPlainText().length())
    {
        return TextEdit->toPlainText();
    }

    return QVariant();
}
//-----------------------------------------------------------------------------
void ISTextEdit::Clear()
{
    TextEdit->clear();
}
//-----------------------------------------------------------------------------
void ISTextEdit::AddText(const QString &Text)
{
    TextEdit->append(Text);
    TextEdit->verticalScrollBar()->setValue(TextEdit->verticalScrollBar()->maximum());
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetExecuteEnter(bool Execute)
{
    TextEdit->SetExecuteEnter(Execute);
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetUppercase(bool uppercase)
{
    if (uppercase)
    {
        connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
    }
    else
    {
        disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
    }
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetLowercase(bool lowercase)
{
    if (lowercase)
    {
        connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
    }
    else
    {
        disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
    }
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetReadOnly(bool read_only)
{
    ISFieldEditBase::SetReadOnly(read_only);
    TextEdit->setReadOnly(read_only);
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetPlaceholderText(const QString &placeholder_text)
{
    TextEdit->setPlaceholderText(placeholder_text);
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetFrameShape(QFrame::Shape FrameShape)
{
    //Если высталено отсутствие рамки - убираем стиль, иначе рамка все равно отображается
    TextEdit->setStyleSheet(FrameShape == QFrame::NoFrame ? QString() : BUFFER_STYLE_SHEET("ISTextEdit"));
    TextEdit->setFrameShape(FrameShape);
}
//-----------------------------------------------------------------------------
void ISTextEdit::SetSizePolicy(QSizePolicy::Policy PolicyHorizontal, QSizePolicy::Policy PolicyVertical)
{
    ISFieldEditBase::SetSizePolicy(PolicyHorizontal, PolicyVertical);
    TextEdit->setSizePolicy(PolicyHorizontal, PolicyVertical);
}
//-----------------------------------------------------------------------------
void ISTextEdit::OnUpperText()
{
    disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);

    QTextCursor TextCursor = TextEdit->textCursor();
    int Position = TextCursor.position();
    TextEdit->setText(TextEdit->toPlainText().toUpper());
    TextCursor.setPosition(Position);
    TextEdit->setTextCursor(TextCursor);

    connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
}
//-----------------------------------------------------------------------------
void ISTextEdit::OnLowerText()
{
    disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);

    QTextCursor TextCursor = TextEdit->textCursor();
    int Position = TextCursor.position();
    TextEdit->setText(TextEdit->toPlainText().toLower());
    TextCursor.setPosition(Position);
    TextEdit->setTextCursor(TextCursor);

    connect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
void ISTextEdit::ResetFontcase()
{
    disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnUpperText);
    disconnect(TextEdit, &ISQTextEdit::textChanged, this, &ISTextEdit::OnLowerText);
}
//-----------------------------------------------------------------------------
ISQTextEdit* ISTextEdit::GetTextEdit()
{
    return TextEdit;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISBIKEdit::ISBIKEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetMaxLength(9);
    SetFixedWidth(ISGui::GetStringWidth("123456789", ISDefines::Gui::FONT_APPLICATION) + 35);
    SetPlaceholderText(LANG("Field.Bik.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISBIKEdit::~ISBIKEdit()
{

}
//-----------------------------------------------------------------------------
bool ISBIKEdit::IsValid() const
{
    int Count = GetValue().toString().length();
    return !Count || Count == 9;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISKPPEdit::ISKPPEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetMaxLength(9);
    SetFixedWidth(ISGui::GetStringWidth("123456789", ISDefines::Gui::FONT_APPLICATION) + 35);
    SetPlaceholderText(LANG("Field.Kpp.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISKPPEdit::~ISKPPEdit()
{

}
//-----------------------------------------------------------------------------
bool ISKPPEdit::IsValid() const
{
    int Count = GetValue().toString().length();
    return !Count || Count == 9;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOGRNEdit::ISOGRNEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetMaxLength(13);
    SetFixedWidth(ISGui::GetStringWidth("1234567890123", ISDefines::Gui::FONT_APPLICATION) + 35);
    SetPlaceholderText(LANG("Field.Ogrn.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISOGRNEdit::~ISOGRNEdit()
{

}
//-----------------------------------------------------------------------------
bool ISOGRNEdit::IsValid() const
{
    int Count = GetValue().toString().length();
    return !Count || Count == 13;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOKPOEdit::ISOKPOEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetMaxLength(8);
    SetFixedWidth(ISGui::GetStringWidth("12345678", ISDefines::Gui::FONT_APPLICATION) + 35);
    SetPlaceholderText(LANG("Field.Okpo.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISOKPOEdit::~ISOKPOEdit()
{

}
//-----------------------------------------------------------------------------
bool ISOKPOEdit::IsValid() const
{
    int Count = GetValue().toString().length();
    return !Count || Count == 8;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISUuidEdit::ISUuidEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetPlaceholderText(UUID_PLACEHOLDER_TEXT);
    SetFixedWidth(ISGui::GetStringWidth(UUID_PLACEHOLDER_TEXT, ISDefines::Gui::FONT_APPLICATION) + 40);
    SetUppercase(true);
    SetSizePolicyHorizontal(QSizePolicy::Maximum);

    ButtonGenerate = new ISServiceButton(BUFFER_ICONS("UUID.Generate"), LANG("ClickFromGenerateNewUID"), this);
    connect(ButtonGenerate, &ISServiceButton::clicked, this, &ISUuidEdit::Generate);
    AddWidgetToRight(ButtonGenerate);
}
//-----------------------------------------------------------------------------
ISUuidEdit::~ISUuidEdit()
{

}
//-----------------------------------------------------------------------------
void ISUuidEdit::SetValue(const QVariant &value)
{
    ISLineEdit::SetValue(value.toString());
}
//-----------------------------------------------------------------------------
bool ISUuidEdit::IsValid() const
{
    //return !ISUuid(GetValue().toString()).isEmpty();
    return true;
}
//-----------------------------------------------------------------------------
void ISUuidEdit::SetReadOnly(bool read_only)
{
    ISLineEdit::SetReadOnly(read_only);
    ButtonGenerate->setVisible(!read_only);
}
//-----------------------------------------------------------------------------
void ISUuidEdit::Generate()
{
    SetValue(QString::fromStdString(GENERATE_UUID()));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISVINEdit::ISVINEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetMaxLength(17);
    SetFixedWidth(ISGui::GetStringWidth("12345678901234567", ISDefines::Gui::FONT_APPLICATION) + 35);
    SetPlaceholderText(LANG("Field.Vin.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISVINEdit::~ISVINEdit()
{

}
//-----------------------------------------------------------------------------
bool ISVINEdit::IsValid() const
{
    int Count = GetValue().toString().length();
    return !Count || Count == 17;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDateTimeEdit::ISDateTimeEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    CreateButtonClear();

    QHBoxLayout *Layout = new QHBoxLayout();
    Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

    QWidget *Widget = new QWidget(this);
    Widget->setLayout(Layout);
    AddWidgetEdit(Widget, this);

    DateEdit = new ISQDateEdit(this);
    connect(DateEdit, &ISQDateEdit::DateChanged, this, &ISDateTimeEdit::ValueChanged);
    Layout->addWidget(DateEdit);

    TimeEdit = new ISQTimeEdit(this);
    connect(TimeEdit, &ISQTimeEdit::TimeChanged, this, &ISDateTimeEdit::ValueChanged);
    Layout->addWidget(TimeEdit);
}
//-----------------------------------------------------------------------------
ISDateTimeEdit::~ISDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetValue(const QVariant &value)
{
    QDateTime DateTime = value.toDateTime();
    if (!DateTime.date().isNull())
    {
        DateEdit->SetDate(DateTime.date());
    }
    if (!DateTime.time().isNull())
    {
        TimeEdit->SetTime(DateTime.time());
    }
}
//-----------------------------------------------------------------------------
QVariant ISDateTimeEdit::GetValue() const
{
    QDate Date = DateEdit->GetDate();
    QTime Time = TimeEdit->GetTime();
    return !Date.isValid() && !Time.isValid() ? QVariant() : QDateTime(DateEdit->GetDate(), TimeEdit->GetTime());
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::Clear()
{
    DateEdit->Clear();
    TimeEdit->Clear();
    emit ValueChange(QVariant());
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetEnabled(bool Enabled)
{
    ISFieldEditBase::SetEnabled(Enabled);
    DateEdit->SetEnabled(Enabled);
    TimeEdit->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetReadOnly(bool read_only)
{
    ISFieldEditBase::SetReadOnly(read_only);
    DateEdit->SetReadOnly(read_only);
    TimeEdit->SetReadOnly(read_only);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetDate(const QDate &Date)
{
    DateEdit->SetDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetTime(const QTime &Time)
{
    TimeEdit->SetTime(Time);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetRangeDate(const QDate &Minimum, const QDate &Maximum)
{
    DateEdit->SetRange(Minimum, Maximum);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMinimumDate(const QDate &Date)
{
    DateEdit->SetMinimumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetMaximumDate(const QDate &Date)
{
    DateEdit->SetMaximumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleDateEdit(bool Visible)
{
    DateEdit->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISDateTimeEdit::SetVisibleTimeEdit(bool Visible)
{
    TimeEdit->setVisible(Visible);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDateEdit::ISDateEdit(QWidget *parent) : ISDateTimeEdit(parent)
{
    SetVisibleTimeEdit(false);
}
//-----------------------------------------------------------------------------
ISDateEdit::~ISDateEdit()
{

}
//-----------------------------------------------------------------------------
void ISDateEdit::SetValue(const QVariant &value)
{
    ISDateTimeEdit::SetValue(QDateTime(value.toDate(), QTime()));
}
//-----------------------------------------------------------------------------
QVariant ISDateEdit::GetValue() const
{
    QDate Date = ISDateTimeEdit::GetValue().toDate();
    return Date.isValid() ? Date : QVariant();
}
//-----------------------------------------------------------------------------
void ISDateEdit::SetMinimumDate(const QDate &Date)
{
    ISDateTimeEdit::SetMinimumDate(Date);
}
//-----------------------------------------------------------------------------
void ISDateEdit::SetMaximumDate(const QDate &Date)
{
    ISDateTimeEdit::SetMaximumDate(Date);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTimeEdit::ISTimeEdit(QWidget *parent) : ISDateTimeEdit(parent)
{
    SetVisibleDateEdit(false);
}
//-----------------------------------------------------------------------------
ISTimeEdit::~ISTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISTimeEdit::SetValue(const QVariant &value)
{
    ISDateTimeEdit::SetValue(QDateTime(QDate(), value.toTime()));
}
//-----------------------------------------------------------------------------
QVariant ISTimeEdit::GetValue() const
{
    QTime Time = ISDateTimeEdit::GetValue().toTime();
    return Time.isValid() ? Time : QVariant();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISBirthdayEdit::ISBirthdayEdit(QWidget *parent) : ISDateEdit(parent)
{
    SetRangeDate(QDate(1900, 1, 1), QDate::currentDate());

    Label = new QLabel(this);
    Label->setFrameShape(QFrame::Shape::Panel);
    Label->setFrameShadow(QFrame::Plain);
    Label->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    Label->setSizePolicy(QSizePolicy::Maximum, Label->sizePolicy().verticalPolicy());
    AddWidgetToRight(Label);

    UpdateLabel(QDate());
    connect(this, &ISBirthdayEdit::DataChanged, [=] { UpdateLabel(GetValue().toDate()); });
}
//-----------------------------------------------------------------------------
ISBirthdayEdit::~ISBirthdayEdit()
{

}
//-----------------------------------------------------------------------------
void ISBirthdayEdit::SetValue(const QVariant &value)
{
    ISDateEdit::SetValue(value);
    UpdateLabel(value.toDate());
}
//-----------------------------------------------------------------------------
void ISBirthdayEdit::UpdateLabel(const QDate &Date)
{
    if (Date.isValid())
    {
        int Age = QDate::currentDate().year() - Date.year();
        if (Date.month() > QDate::currentDate().month() || (Date.month() == QDate::currentDate().month() && Date.day() > QDate::currentDate().day()))
        {
            --Age;
        }
        Label->setText(LANG("Age") + ": " + QString::number(Age));
    }
    else
    {
        Label->setText(LANG("Age") + ": " + LANG("AgeNotSelected"));
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISINNEdit::ISINNEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetPlaceholderText(LANG("Field.Inn.PlaceholderText"));
    SetMaxLength(12);
    SetFixedWidth(ISGui::GetStringWidth("123456789012", ISDefines::Gui::FONT_APPLICATION) + 50);

    ButtonSearch = new ISServiceButton(BUFFER_ICONS("Taxation"), LANG("SearchFromINN"), this);
    ButtonSearch->setVisible(false);
    ButtonSearch->setEnabled(false);
    ButtonSearch->setFlat(true);
    ButtonSearch->setFocusPolicy(Qt::NoFocus);
    connect(ButtonSearch, &ISServiceButton::clicked, this, &ISINNEdit::SearchFromINN);
    AddWidgetToRight(ButtonSearch);
}
//-----------------------------------------------------------------------------
ISINNEdit::~ISINNEdit()
{

}
//-----------------------------------------------------------------------------
bool ISINNEdit::IsValid() const
{
    int Count = GetValue().toString().length();
    return !Count || Count == 10 || Count == 12;
}
//-----------------------------------------------------------------------------
void ISINNEdit::SetEnabledSearch(bool Enabled)
{
    ButtonSearch->setVisible(Enabled);
}
//-----------------------------------------------------------------------------
void ISINNEdit::TextChanged(const QString &Text)
{
    ISLineEdit::TextChanged(Text);
    ButtonSearch->setEnabled(!Text.isEmpty() && IsValid());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISWeekDayEdit::ISWeekDayEdit(QWidget *parent) : ISComboEdit(parent)
{
    SetEditable(false);
    AddItem(LANG("DayOfWeek.Everyday"), QVariant());
    AddItem(LANG("DayOfWeek.Monday"), Qt::Monday);
    AddItem(LANG("DayOfWeek.Tuesday"), Qt::Tuesday);
    AddItem(LANG("DayOfWeek.Wednesday"), Qt::Wednesday);
    AddItem(LANG("DayOfWeek.Thursday"), Qt::Thursday);
    AddItem(LANG("DayOfWeek.Friday"), Qt::Friday);
    AddItem(LANG("DayOfWeek.Saturday"), Qt::Saturday);
    AddItem(LANG("DayOfWeek.Sunday"), Qt::Sunday);
}
//-----------------------------------------------------------------------------
ISWeekDayEdit::~ISWeekDayEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISPathEditDir::ISPathEditDir(QWidget *parent) : ISLineEdit(parent)
{
    SetReadOnly(true);
    SetVisibleClear(true); //Чтобы кнопка очистки отображалась, порядок SetReadOnly и SetVisibleClear должен быть именно такой
    SetFocusPolicy(Qt::NoFocus);

    ISPushButton *ButtonSelect = new ISPushButton(this);
    ButtonSelect->setText(LANG("Overview"));
    ButtonSelect->setToolTip(LANG("SelectFolder"));
    connect(ButtonSelect, &ISPushButton::clicked, this, &ISPathEditDir::SelectDir);
    AddWidgetToRight(ButtonSelect);

    ButtonOpenDir = new ISServiceButton(BUFFER_ICONS("FolderClosed"), LANG("OpenFolder"), this);
    ButtonOpenDir->setEnabled(false);
    connect(ButtonOpenDir, &ISPushButton::clicked, this, &ISPathEditDir::OpenDir);
    AddWidgetToRight(ButtonOpenDir);
}
//-----------------------------------------------------------------------------
ISPathEditDir::~ISPathEditDir()
{

}
//-----------------------------------------------------------------------------
void ISPathEditDir::SetValue(const QVariant &value)
{
    ISLineEdit::SetValue(value);
    SetToolTip(value.toString());
    ButtonOpenDir->setEnabled(!value.toString().isEmpty());
}
//-----------------------------------------------------------------------------
void ISPathEditDir::Clear()
{
    ISLineEdit::Clear();
    ButtonOpenDir->setEnabled(false);
}
//-----------------------------------------------------------------------------
void ISPathEditDir::SelectDir()
{
    QString FolderPath = ISFileDialog::GetDirectoryPath(this);
    if (!FolderPath.isEmpty())
    {
        SetValue(FolderPath);
    }
}
//-----------------------------------------------------------------------------
void ISPathEditDir::OpenDir()
{
    if (!ISGui::OpenFolder(GetValue().toString()))
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Error.ErrorOpenedFolderPath").arg(GetValue().toString()));
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISPathEditFile::ISPathEditFile(QWidget *parent) : ISLineEdit(parent)
{
    //Чтобы кнопка очистки отображалась, порядок SetReadOnly и SetVisibleClear доллжен быть именно такой
    SetReadOnly(true);
    SetVisibleClear(true);

    SetFocusPolicy(Qt::NoFocus);
    SetPlaceholderText(LANG("ClickOverviewFromSelectFile"));

    ISPushButton *ButtonSelect = new ISPushButton(this);
    ButtonSelect->setText(LANG("Overview"));
    ButtonSelect->setToolTip(LANG("SelectFile"));
    connect(ButtonSelect, &ISPushButton::clicked, this, &ISPathEditFile::SelectFile);
    AddWidgetToRight(ButtonSelect);
}
//-----------------------------------------------------------------------------
ISPathEditFile::~ISPathEditFile()
{

}
//-----------------------------------------------------------------------------
void ISPathEditFile::SetValue(const QVariant &value)
{
    ISLineEdit::SetValue(value);
    SetToolTip(value.toString());
}
//-----------------------------------------------------------------------------
void ISPathEditFile::SetFilterFile(const QString &filter_file)
{
    FilterFile = filter_file;
}
//-----------------------------------------------------------------------------
void ISPathEditFile::SelectFile()
{
    QString Path = ISFileDialog::GetOpenFileName(this, GetValue().toString(), FilterFile);
    if (!Path.isEmpty())
    {
        SetValue(Path);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISUrlEdit::ISUrlEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetValidator(new QRegExpValidator(QRegExp(REG_EXP_URL), this));
    connect(this, &ISUrlEdit::DataChanged, this, &ISUrlEdit::UrlChanged);

    ButtonUrl = new ISServiceButton(BUFFER_ICONS("Url"), LANG("OpenUrlLink"), this);
    connect(ButtonUrl, &ISServiceButton::clicked, this, &ISUrlEdit::OpenUrl);
    AddWidgetToRight(ButtonUrl);
}
//-----------------------------------------------------------------------------
ISUrlEdit::~ISUrlEdit()
{

}
//-----------------------------------------------------------------------------
void ISUrlEdit::OpenUrl()
{
    QString UrlString = GetValue().toString();
    if (!UrlString.isEmpty())
    {
        if (ISGui::OpenUrl(UrlString))
        {
            ISPopupMessage *PopupMessage = new ISPopupMessage(nullptr);
            PopupMessage->SetTitle(LANG("OpeningUrl"));
            PopupMessage->SetMessage(UrlString);
            PopupMessage->show();
        }
        else
        {
            ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.OpenUrl.Failed").arg(UrlString));
        }
    }
    else
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.OpenUrl.Empty"));
        BlinkRed();
    }
}
//-----------------------------------------------------------------------------
void ISUrlEdit::UrlChanged()
{
    ButtonUrl->setEnabled(!GetValue().toString().isEmpty());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMonthEdit::ISMonthEdit(QWidget *parent) : ISComboEdit(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetEditable(false);

    AddItem(LANG("Month.NotSelected"), QVariant());
    AddItem(LANG("Month.January"), 1);
    AddItem(LANG("Month.February"), 2);
    AddItem(LANG("Month.March"), 3);
    AddItem(LANG("Month.April"), 4);
    AddItem(LANG("Month.May"), 5);
    AddItem(LANG("Month.June"), 6);
    AddItem(LANG("Month.July"), 7);
    AddItem(LANG("Month.August"), 8);
    AddItem(LANG("Month.September"), 9);
    AddItem(LANG("Month.October"), 10);
    AddItem(LANG("Month.November"), 11);
    AddItem(LANG("Month.December"), 12);
}
//-----------------------------------------------------------------------------
ISMonthEdit::~ISMonthEdit()
{

}
//-----------------------------------------------------------------------------
void ISMonthEdit::SelectCurrentMonth()
{
    SetValue(QDate::currentDate().month());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISEMailEdit::ISEMailEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetIcon(BUFFER_ICONS("EMail.FieldEdit.Acceptable"));
    SetPlaceholderText(LANG("Field.EMail.PlaceholderText"));

    ISServiceButton *ButtonSend = new ISServiceButton(BUFFER_ICONS("EMailSend"), LANG("Send"), this);
    ButtonSend->setVisible(false);
    ButtonSend->setFocusPolicy(Qt::NoFocus);
    connect(ButtonSend, &ISServiceButton::clicked, this, &ISEMailEdit::Send);
    AddWidgetToRight(ButtonSend);
}
//-----------------------------------------------------------------------------
ISEMailEdit::~ISEMailEdit()
{

}
//-----------------------------------------------------------------------------
void ISEMailEdit::Send()
{
    QString EMail = GetValue().toString();
    if (!EMail.isEmpty())
    {
        ISGui::SetWaitGlobalCursor(true);
        QDesktopServices::openUrl(QUrl("mailto:" + EMail));
        ISGui::SetWaitGlobalCursor(false);
    }
    else
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.EMailIsEmpty"));
        BlinkRed();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDivisionCodeEdit::ISDivisionCodeEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetInputMask("000-000;_");
}
//-----------------------------------------------------------------------------
ISDivisionCodeEdit::~ISDivisionCodeEdit()
{

}
//-----------------------------------------------------------------------------
bool ISDivisionCodeEdit::IsValid() const
{
    return GetValue().toString().size() == 7;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTaskPriorityEdit::ISTaskPriorityEdit(QWidget *parent) : ISRadioEdit(parent)
{
    QRadioButton *ButtonLow = nullptr;

    QVariantList TaskPriorityList = ISBuffer::Instance().GetTaskPriority();
    for (const QVariant &Variant : TaskPriorityList)
    {
        QVariantMap TaskPriorityMap = Variant.toMap();
        QRadioButton *ButtonPriority = new QRadioButton(TaskPriorityMap["LocalName"].toString(), this);
        ButtonPriority->setToolTip(TaskPriorityMap["ToolTip"].toString());
        ButtonPriority->setIcon(BUFFER_ICONS(TaskPriorityMap["Icon"].toString()));
        ButtonPriority->setStyleSheet(BUFFER_STYLE_SHEET(TaskPriorityMap["StyleSheet"].toString()));
        ButtonPriority->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
        AddButton(ButtonPriority, TaskPriorityMap["ID"]);

        if (!ButtonLow)
        {
            ButtonLow = ButtonPriority;
        }
    }

    //Установка низкого приоритета задачи по умолчанию
    ButtonLow->setChecked(true);
    SetModificationFlag(true);
}
//-----------------------------------------------------------------------------
ISTaskPriorityEdit::~ISTaskPriorityEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTaskImportantEdit::ISTaskImportantEdit(QWidget *parent) : ISCheckEdit(parent)
{
    GetCheckBox()->setIcon(BUFFER_ICONS("Task.Important.Unchecked"));
    GetCheckBox()->setToolTip(LANG("ISImportantEditToolTip"));
    connect(this, &ISTaskImportantEdit::ValueChange, this, &ISTaskImportantEdit::ImportantChanged);
}
//-----------------------------------------------------------------------------
ISTaskImportantEdit::~ISTaskImportantEdit()
{

}
//-----------------------------------------------------------------------------
void ISTaskImportantEdit::ImportantChanged(const QVariant &value)
{
    GetCheckBox()->setIcon(value.toBool() ? BUFFER_ICONS("Task.Important.Checked") : BUFFER_ICONS("Task.Important.Unchecked"));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTaskDescriptionEdit::ISTaskDescriptionEdit(QWidget *parent) : ISTextEdit(parent)
{
    GetTextEdit()->setMaximumHeight(125);
}
//-----------------------------------------------------------------------------
ISTaskDescriptionEdit::~ISTaskDescriptionEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDoubleEdit::ISDoubleEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetValidator(new ISDoubleValidator(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA), this));
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
}
//-----------------------------------------------------------------------------
ISDoubleEdit::~ISDoubleEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMoneyEdit::ISMoneyEdit(QWidget *parent) : ISLineEdit(parent)
{
    SetValidator(new ISMoneyValidator(this));
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
}
//-----------------------------------------------------------------------------
ISMoneyEdit::~ISMoneyEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboTimeEdit::ISComboTimeEdit(QWidget *parent) : ISComboEdit(parent)
{
    SetEditable(false);

    AddItem(LANG("Minute_3"), 3);
    AddItem(LANG("Minute_5"), 5);
    AddItem(LANG("Minute_10"), 10);
    AddItem(LANG("Minute_15"), 15);
    AddItem(LANG("Minute_30"), 30);
    AddItem(LANG("Minute_45"), 45);
    AddItem(LANG("Minute_60"), 60);
    AddItem(LANG("Minute_120"), 120);
    AddItem(LANG("Minute_180"), 180);
    AddItem(LANG("Minute_240"), 240);
    AddItem(LANG("Minute_300"), 300);
    AddItem(LANG("Minute_360"), 360);
}
//-----------------------------------------------------------------------------
ISComboTimeEdit::~ISComboTimeEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISIntegerEdit::ISIntegerEdit(QWidget *parent)
    : ISLineEdit(parent),
    IntValidator(new ISIntValidator(this))
{
    SetValidator(IntValidator);
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
}
//-----------------------------------------------------------------------------
ISIntegerEdit::~ISIntegerEdit()
{

}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetValue(const QVariant &value)
{
    ISLineEdit::SetValue(value.toString().isEmpty() ? QVariant() : value.toInt());
}
//-----------------------------------------------------------------------------
QVariant ISIntegerEdit::GetValue() const
{
    QString Value = ISLineEdit::GetValue().toString();
    return Value.isEmpty() ? QVariant() : QVariant(Value.toInt());
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMinimum(int Minimum)
{
    int Maximum = IntValidator->top(); //Запоминаем значение максимального предела
    if (IntValidator)
    {
        delete IntValidator;
    }

    IntValidator = new ISIntValidator(Minimum, Maximum, this);
    SetValidator(IntValidator);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMaximum(int Maximum)
{
    int Minimum = IntValidator->bottom(); //Запоминаем значение нижнего предела
    if (IntValidator)
    {
        delete IntValidator;
    }

    IntValidator = new ISIntValidator(Minimum, Maximum, this);
    SetValidator(IntValidator);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetRange(int Minimum, int Maximum)
{
    SetMinimum(Minimum);
    SetMaximum(Maximum);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::ResetRange()
{
    SetRange(0, 0);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISYearEdit::ISYearEdit(QWidget *parent) : ISIntegerEdit(parent)
{
    SetMinimum(YEAR_MINIMUM);
    SetMaximum(YEAR_MAXIMUM);

    ISServiceButton *ButtonCurrentYear = new ISServiceButton(BUFFER_ICONS("Calendar"), LANG("CurrentYear"), this);
    ButtonCurrentYear->setFocusPolicy(Qt::NoFocus);
    connect(ButtonCurrentYear, &ISServiceButton::clicked, this, &ISYearEdit::SelectCurrentYear);
    AddWidgetToRight(ButtonCurrentYear);
}
//-----------------------------------------------------------------------------
ISYearEdit::~ISYearEdit()
{

}
//-----------------------------------------------------------------------------
void ISYearEdit::SelectCurrentYear()
{
    SetValue(QDate::currentDate().year());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISPhoneEdit::ISPhoneEdit(QWidget *parent) : ISLineEdit(parent)
{
    QString InputMask = "(000) 000-00-00;_";
    SetInputMask(InputMask);
    SetFixedWidth(ISGui::GetStringWidth(InputMask, ISDefines::Gui::FONT_APPLICATION) + 50);
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    SetIcon(BUFFER_ICONS("PhoneEdit"));
}
//-----------------------------------------------------------------------------
ISPhoneEdit::~ISPhoneEdit()
{

}
//-----------------------------------------------------------------------------
QVariant ISPhoneEdit::GetValue() const
{
    QString String = ISLineEdit::GetValue().toString();
    ISAlgorithm::FormatPhoneNumber(String);
    return String.isEmpty() ? QVariant() : String;
}
//-----------------------------------------------------------------------------
bool ISPhoneEdit::IsValid() const
{
    QString Phone = GetValue().toString();
    return Phone.size() == 10 || Phone.isEmpty();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSearchEdit::ISSearchEdit(QWidget *parent, const QString &PlaceholderText) : ISLineEdit(parent)
{
    SetPlaceholderText(PlaceholderText.isEmpty() ? LANG("EnteringSearchQuery") : PlaceholderText);
    SetIcon(BUFFER_ICONS("Search"));
}
//-----------------------------------------------------------------------------
ISSearchEdit::~ISSearchEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISColorEdit::ISColorEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    CreateButtonClear();

    QGroupBox *GroupBox = new QGroupBox(this);
    GroupBox->setLayout(new QHBoxLayout());
    GroupBox->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
    GroupBox->setMinimumWidth(ISPUSHBUTTON_MINIMUM_WIDTH);
    GroupBox->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    AddWidgetEdit(GroupBox, this);

    WidgetColor = new QWidget(this);
    WidgetColor->setAutoFillBackground(true);
    GroupBox->layout()->addWidget(WidgetColor);

    ISServiceButton *ButtonSelectColor = new ISServiceButton(BUFFER_ICONS("Color"), LANG("SelectColor"), this);
    ButtonSelectColor->setFocusPolicy(Qt::NoFocus);
    connect(ButtonSelectColor, &ISServiceButton::clicked, this, &ISColorEdit::SelectColor);
    AddWidgetToRight(ButtonSelectColor);
}
//-----------------------------------------------------------------------------
ISColorEdit::~ISColorEdit()
{

}
//-----------------------------------------------------------------------------
void ISColorEdit::SetValue(const QVariant &value)
{
    if (value.toString().length())
    {
        QColor Color(value.toString());
        QPalette Palette = WidgetColor->palette();
        Palette.setColor(QPalette::Background, Color);
        WidgetColor->setPalette(Palette);
        WidgetColor->setToolTip(QString("RGB(%1, %2, %3)").arg(Color.red()).arg(Color.green()).arg(Color.blue()));
    }
}
//-----------------------------------------------------------------------------
QVariant ISColorEdit::GetValue() const
{
    return WidgetColor->palette().color(QPalette::Background);
}
//-----------------------------------------------------------------------------
void ISColorEdit::Clear()
{
    WidgetColor->setPalette(QPalette());
    WidgetColor->setToolTip(QString());
}
//-----------------------------------------------------------------------------
void ISColorEdit::SetReadOnly(bool read_only)
{
    WidgetColor->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
void ISColorEdit::SelectColor()
{
    QColor Color = QColorDialog::getColor(WidgetColor->palette().color(QPalette::Background), this, LANG("SelectedColor"), QColorDialog::DontUseNativeDialog);
    QString ColorName = Color.name();
    if (ColorName != COLOR_STANDART)
    {
        SetValue(ColorName);
        emit ValueChanged();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISFileEdit::ISFileEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);
    CreateButtonClear();

    ButtonFile = new ISPushButton(this);
    ButtonFile->setCursor(CURSOR_POINTING_HAND);
    ButtonFile->setText(LANG("FileNotSelected"));
    ButtonFile->setToolTip(LANG("ClickFromSelectFile"));
    connect(ButtonFile, &ISPushButton::clicked, this, &ISFileEdit::SelectFile);
    AddWidgetEdit(ButtonFile, this);

    Menu = new QMenu(this);
    Menu->addAction(BUFFER_ICONS("Select"), LANG("Overview"), this, &ISFileEdit::SelectFile);
    ActionSave = Menu->addAction(BUFFER_ICONS("Save"), LANG("SaveToDisk"), this, &ISFileEdit::Save);
    ActionRename = Menu->addAction(BUFFER_ICONS("Edit"), LANG("Rename"), this, &ISFileEdit::Rename);

    ActionSave->setEnabled(false);
    ActionRename->setEnabled(false);
}
//-----------------------------------------------------------------------------
ISFileEdit::~ISFileEdit()
{

}
//-----------------------------------------------------------------------------
void ISFileEdit::SetValue(const QVariant &value)
{
    QJsonParseError JsonParseError;
    VariantMap = ISSystem::JsonStringToVariantMap(value.toString(), JsonParseError);
    if (JsonParseError.error == QJsonParseError::NoError)
    {
        ButtonFile->setMenu(Menu);
        ButtonFile->setText(VariantMap[FILE_EDIT_PROPERTY_NAME].toString());
        ActionSave->setEnabled(true);
        ActionRename->setEnabled(true);

        QPixmap Pixmap;
        if (Pixmap.loadFromData(QByteArray::fromBase64(VariantMap[FILE_EDIT_PROPERTY_LOGO].toString().toUtf8())))
        {
            ButtonFile->setIcon(QIcon(Pixmap));
        }
    }
    else
    {
        ButtonFile->setText(JsonParseError.errorString());
    }
}
//-----------------------------------------------------------------------------
QVariant ISFileEdit::GetValue() const
{
    return VariantMap.isEmpty() ? QVariant() : ISSystem::VariantMapToJsonString(VariantMap, QJsonDocument::Compact).simplified();
}
//-----------------------------------------------------------------------------
void ISFileEdit::Clear()
{
    VariantMap.clear();
    ButtonFile->setMenu(nullptr);
    ButtonFile->setIcon(QIcon());
    ButtonFile->setText(LANG("FileNotSelected"));
    ActionSave->setEnabled(false);
    ActionRename->setEnabled(false);
    emit ValueChanged();
}
//-----------------------------------------------------------------------------
void ISFileEdit::SelectFile()
{
    QString FilePath = ISFileDialog::GetOpenFileName(this);
    if (!FilePath.isEmpty())
    {
        QFileInfo FileInfo(FilePath);
        QFile File(FilePath);
        if (File.size() > (((1000 * 1024) * SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE))))
        {
            ISMessageBox::ShowWarning(this, LANG("Message.Warning.InsertingFileSizeVeryBig").arg(FileInfo.fileName()).arg(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE)));
            return;
        }

        if (File.open(QIODevice::ReadOnly))
        {
            ISGui::SetWaitGlobalCursor(true);
            QIcon IconFile = ISGui::GetIconFile(FilePath);
            VariantMap[FILE_EDIT_PROPERTY_NAME] = FileInfo.fileName();
            VariantMap[FILE_EDIT_PROPERTY_LOGO] = ISGui::PixmapToByteArray(IconFile.pixmap(ISDefines::Gui::SIZE_32_32)).toBase64();
            VariantMap[FILE_EDIT_PROPERTY_DATA] = File.readAll().toBase64();
            File.close();
            ButtonFile->setMenu(Menu);
            ButtonFile->setIcon(IconFile);
            ButtonFile->setText(FileInfo.fileName());
            ActionSave->setEnabled(true);
            ActionRename->setEnabled(true);
            emit ValueChanged();
            ISGui::SetWaitGlobalCursor(false);
        }
        else
        {
            ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FileInfo.fileName()), File.errorString());
        }
    }
}
//-----------------------------------------------------------------------------
void ISFileEdit::Save()
{
    QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(QFileInfo(VariantMap[FILE_EDIT_PROPERTY_NAME].toString()).suffix()), QFileInfo(VariantMap[FILE_EDIT_PROPERTY_NAME].toString()).baseName());
    if (!FilePath.isEmpty())
    {
        QFile File(FilePath);
        if (File.open(QIODevice::WriteOnly))
        {
            File.write(QByteArray::fromBase64(VariantMap[FILE_EDIT_PROPERTY_DATA].toString().toUtf8()));
            File.close();

            if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.FileSaved")))
            {
                if (!ISGui::OpenFile(FilePath))
                {
                    ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FilePath));
                }
            }
        }
        else
        {
            ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(File.fileName()), File.errorString());
        }
    }
}
//-----------------------------------------------------------------------------
void ISFileEdit::Rename()
{
    QFileInfo FileInfo(VariantMap[FILE_EDIT_PROPERTY_NAME].toString());
    QString FileName = ISInputDialog::GetString(LANG("Renaming"), LANG("EnterFileName") + ':', FileInfo.baseName());
    if (!FileName.isEmpty())
    {
        FileName += SYMBOL_POINT + FileInfo.suffix();
        ButtonFile->setText(FileName);
        VariantMap[FILE_EDIT_PROPERTY_NAME] = FileName;
        emit ValueChanged();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISImageEdit::ISImageEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    CreateButtonClear();
    SetSizePolicyHorizontal(QSizePolicy::Maximum);

    ImageWidget = new ISImageWidget(this);
    ImageWidget->setFixedSize(QSize(150, 150));
    ImageWidget->setCursor(CURSOR_POINTING_HAND);
    ImageWidget->setToolTip(LANG("ClickRightButtonMouseForCallContextMenu"));
    connect(ImageWidget, &ISImageWidget::ImageChanged, this, &ISImageEdit::ValueChanged);
    AddWidgetEdit(ImageWidget, this);
}
//-----------------------------------------------------------------------------
ISImageEdit::~ISImageEdit()
{

}
//-----------------------------------------------------------------------------
void ISImageEdit::SetValue(const QVariant &value)
{
    value.isValid() ? ImageWidget->SetByteArray(QByteArray::fromBase64(value.toByteArray())) : ImageWidget->Clear();
}
//-----------------------------------------------------------------------------
QVariant ISImageEdit::GetValue() const
{
    QByteArray ByteArray = ImageWidget->GetImage();
    return ByteArray.isEmpty() ? QVariant() : ByteArray.toBase64();
}
//-----------------------------------------------------------------------------
void ISImageEdit::Clear()
{
    ImageWidget->Clear();
}
//-----------------------------------------------------------------------------
void ISImageEdit::SetReadOnly(bool read_only)
{
    ImageWidget->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISVolumeEdit::ISVolumeEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    SetSizePolicyHorizontal(QSizePolicy::Maximum);

    QLabel *LabelImageMinimum = new QLabel(this);
    LabelImageMinimum->setPixmap(BUFFER_ICONS("Volume.Minimum").pixmap(ISDefines::Gui::SIZE_20_20));
    LabelImageMinimum->setContentsMargins(0, 0, 0, 5);
    AddWidgetToLeft(LabelImageMinimum);

    Slider = new QSlider(Qt::Horizontal, this);
    Slider->setMinimum(0);
    Slider->setMaximum(100);
    Slider->setTickPosition(QSlider::TicksBelow);
    Slider->setTickInterval(10);
    connect(Slider, &QSlider::valueChanged, this, &ISVolumeEdit::ValueChanged);
    AddWidgetEdit(Slider, this);

    QLabel *LabelImageMaximum = new QLabel(this);
    LabelImageMaximum->setPixmap(BUFFER_ICONS("Volume.Maximum").pixmap(ISDefines::Gui::SIZE_20_20));
    LabelImageMaximum->setContentsMargins(0, 0, 0, 5);
    AddWidgetToRight(LabelImageMaximum);

    Label = new QLabel(this);
    Label->setText("0%");
    AddWidgetToRight(Label);

    ButtonCheck = new ISServiceButton(BUFFER_ICONS("Volume.Check"), LANG("ClickFromCheckLevelVolume"), this);
    ButtonCheck->setFocusPolicy(Qt::NoFocus);
    connect(ButtonCheck, &ISServiceButton::clicked, this, &ISVolumeEdit::VolumeCheck);
    AddWidgetToRight(ButtonCheck);

    SetValue(0);

    MediaPlayer = new QMediaPlayer(this);
    MediaPlayer->setMedia(QUrl("qrc:/Audio/VolumeChecking.wav"));
    connect(MediaPlayer, &QMediaPlayer::stateChanged, this, &ISVolumeEdit::MediaStateChanged);
}
//-----------------------------------------------------------------------------
ISVolumeEdit::~ISVolumeEdit()
{

}
//-----------------------------------------------------------------------------
void ISVolumeEdit::SetValue(const QVariant &value)
{
    Slider->setValue(value.toInt());
}
//-----------------------------------------------------------------------------
QVariant ISVolumeEdit::GetValue() const
{
    return Slider->value();
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::Clear()
{
    ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::ValueChanged()
{
    ISFieldEditBase::ValueChanged();
    Label->setText(QString("%1%").arg(Slider->value()));
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::VolumeCheck()
{
    ISGui::SetWaitGlobalCursor(true);
    ButtonCheck->setEnabled(false);
    Slider->setEnabled(false);

    MediaPlayer->setVolume(Slider->value());
    MediaPlayer->play();
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::MediaStateChanged(QMediaPlayer::State NewState)
{
    if (NewState == QMediaPlayer::StoppedState)
    {
        ISGui::SetWaitGlobalCursor(false);
        ButtonCheck->setEnabled(true);
        Slider->setEnabled(true);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISListEditPopup::ISListEditPopup(PMetaForeign *meta_foreign, QWidget *ComboBox)
    : ISInterfaceForm(ComboBox),
    MetaForeign(meta_foreign),
    MetaTableForeign(ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass))
{
    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose, false);

    QVBoxLayout *LayoutFrame = new QVBoxLayout();
    LayoutFrame->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);

    QFrame *Frame = new QFrame(this);
    Frame->setFrameShape(QFrame::Box);
    Frame->setFrameShadow(QFrame::Raised);
    Frame->setLayout(LayoutFrame);
    GetMainLayout()->addWidget(Frame);

    LineEdit = new ISLineEdit(this);
    LineEdit->SetPlaceholderText(LANG("Search"));
    LineEdit->SetIcon(BUFFER_ICONS("Search"));
    connect(LineEdit, &ISLineEdit::ValueChange, this, &ISListEditPopup::Search);
    LayoutFrame->addWidget(LineEdit);

    LabelName = new QLabel(MetaTableForeign->LocalListName + ':', this);
    LabelName->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
    LabelName->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
    LayoutFrame->addWidget(LabelName);

    ListWidget = new ISListWidget(this);
    ListWidget->setCursor(CURSOR_POINTING_HAND);
    connect(ListWidget, &QListWidget::itemClicked, this, &ISListEditPopup::ItemClicked);
    LayoutFrame->addWidget(ListWidget);

    QStatusBar *StatusBar = new QStatusBar(this);
    LayoutFrame->addWidget(StatusBar);

    LabelCountRow = new QLabel(StatusBar);
    StatusBar->addWidget(LabelCountRow);

    LabelSearch = new QLabel(StatusBar);
    LabelSearch->setVisible(false);
    StatusBar->addWidget(LabelSearch);

    if (!MetaTableForeign->ShowOnly)
    {
        ISPushButton *ButtonAdd = new ISPushButton(BUFFER_ICONS("Add"), LANG("AddRecord"), this);
        connect(ButtonAdd, &ISServiceButton::clicked, this, &ISListEditPopup::Add);
        StatusBar->addWidget(ButtonAdd);
    }

    ISPushButton *ButtonHide = new ISPushButton(BUFFER_ICONS("ArrowUp"), LANG("Hide"), StatusBar);
    ButtonHide->setFlat(true);
    connect(ButtonHide, &ISPushButton::clicked, this, &ISListEditPopup::hide);
    StatusBar->addPermanentWidget(ButtonHide);

    LabelEmpty = new QLabel(LANG("ListIsEmpty"), this);
    LabelEmpty->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
    LabelEmpty->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
    LabelEmpty->setVisible(false);
}
//-----------------------------------------------------------------------------
ISListEditPopup::~ISListEditPopup()
{

}
//-----------------------------------------------------------------------------
void ISListEditPopup::showEvent(QShowEvent *e)
{
    ISGui::SetWaitGlobalCursor(true);
    LoadDataFromQuery();
    LineEdit->SetFocus();
    ISInterfaceForm::showEvent(e);
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::hideEvent(QHideEvent *e)
{
    ISGui::SetWaitGlobalCursor(true);
    ListWidget->Clear();
    LineEdit->Clear();
    CurrentValue.clear();
    ISInterfaceForm::hideEvent(e);
    ISGui::SetWaitGlobalCursor(false);
    emit Hided();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::paintEvent(QPaintEvent *e)
{
    ISInterfaceForm::paintEvent(e);
    if (LabelEmpty->isVisible())
    {
        QPoint CenterPoint = ListWidget->frameGeometry().center();
        CenterPoint.setX(CenterPoint.x() - (LabelEmpty->width() / 2));
        CenterPoint.setY(CenterPoint.y() - (LabelEmpty->height() / 2));
        LabelEmpty->move(CenterPoint);
        LabelEmpty->adjustSize();
    }
}
//-----------------------------------------------------------------------------
void ISListEditPopup::SetCurrentValue(const QVariant &current_value)
{
    CurrentValue = current_value;
}
//-----------------------------------------------------------------------------
void ISListEditPopup::Search(const QVariant &value)
{
    ISGui::SetWaitGlobalCursor(true);
    QString SearchValue = value.toString().toLower();
    if (SearchValue.isEmpty())
    {
        ListWidget->SetVisibleItems(true);
        LabelSearch->setVisible(false);
        LabelSearch->clear();
    }
    else
    {
        int Founded = 0;
        for (int i = 0; i < ListWidget->count(); ++i)
        {
            QListWidgetItem *ListWidgetItem = ListWidget->item(i);
            if (ListWidgetItem->text().toLower().contains(SearchValue))
            {
                ++Founded;
                ListWidget->setItemHidden(ListWidgetItem, false);
            }
            else
            {
                ListWidget->setItemHidden(ListWidgetItem, true);
            }
        }

        if (Founded)
        {
            QListWidgetItem *ListWidgetItem = ListWidget->BeginItem();
            ListWidget->setItemSelected(ListWidgetItem, true);
            ListWidget->setCurrentItem(ListWidgetItem);
        }
        LabelSearch->setVisible(true);
        LabelSearch->setText(LANG("Founded") + ": " + QString::number(Founded));
    }
    ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::ItemClicked(QListWidgetItem *ListWidgetItem)
{
    emit Selected(ListWidgetItem->data(Qt::UserRole), ListWidgetItem->text());
    hide();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::Add()
{
    hide();
    ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::New, MetaTableForeign->Name);
    connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISListEditPopup::Created);
    ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::LoadDataFromQuery()
{
    ISTcpQuery qGetForeignList(API_GET_FOREIGN_LIST);
    qGetForeignList.BindValue("TableName", MetaForeign->TableName);
    qGetForeignList.BindValue("FieldName", MetaForeign->Field);
    if (qGetForeignList.Execute())
    {
        QListWidgetItem *CurrentItem = nullptr;
        QVariantList AnswerList = qGetForeignList.TakeAnswer()["List"].toList();
        for (const QVariant &Variant : AnswerList)
        {
            QVariantMap VariantMap = Variant.toMap();
            QVariant ID = VariantMap["ID"];
            QString Value = VariantMap["Value"].toString();

            QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
            ListWidgetItem->setText(Value);
            ListWidgetItem->setData(Qt::UserRole, ID);
            ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 25));

            if (CurrentValue == ID)
            {
                CurrentItem = ListWidgetItem;
            }
        }

        if (CurrentItem)
        {
            ListWidget->setItemSelected(CurrentItem, true);
            ListWidget->scrollToItem(CurrentItem, QAbstractItemView::ScrollHint::PositionAtCenter);
            CurrentItem->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
            CurrentItem->setSelected(true);
        }
        LabelCountRow->setText(LANG("RecordsCount") + ": " + QString::number(ListWidget->count()));
        LabelEmpty->setVisible(!ListWidget->count());
    }
    else
    {
        ISMessageBox::ShowCritical(this, qGetForeignList.GetErrorString());
    }
}
//-----------------------------------------------------------------------------
void ISListEditPopup::EnterClicked()
{
    QListWidgetItem *ListWidgetItem = ListWidget->currentItem();
    if (ListWidgetItem)
    {
        ItemClicked(ListWidgetItem);
    }
}
//-----------------------------------------------------------------------------
void ISListEditPopup::EscapeClicked()
{
    hide();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISListEdit::ISListEdit(QWidget *parent) : ISFieldEditBase(parent)
{
    CreateButtonClear();

    ButtonMain = new ISPushButton(this);
    ButtonMain->setText(LANG("NotSelected"));
    ButtonMain->setToolTip(LANG("ClickFromShowList"));
    ButtonMain->setIcon(BUFFER_ICONS("ArrowDown"));
    ButtonMain->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    ButtonMain->setCursor(CURSOR_POINTING_HAND);
    ButtonMain->setStyleSheet(ButtonMain->styleSheet() + BUFFER_STYLE_SHEET("QPushButtonListEdit"));
    ButtonMain->setCheckable(true);
    connect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup);
    AddWidgetEdit(ButtonMain, this);

    ButtonList = new QToolButton(this);
    ButtonList->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
    ButtonList->setIcon(BUFFER_ICONS("Search"));
    ButtonList->setToolTip(LANG("OpenList"));
    ButtonList->setCursor(CURSOR_POINTING_HAND);
    ButtonList->setAutoRaise(true);
    ButtonList->setFocusPolicy(Qt::NoFocus);
    connect(ButtonList, &QToolButton::clicked, this, &ISListEdit::ShowListForm);
    AddWidgetToRight(ButtonList);
}
//-----------------------------------------------------------------------------
ISListEdit::~ISListEdit()
{

}
//-----------------------------------------------------------------------------
void ISListEdit::SetValue(const QVariant &value)
{
    //Анализируем значение от сервера
    QString String = value.toString();
    if (String.isEmpty()) //Если значение пустое - выходим
    {
        return;
    }

    int Pos = String.indexOf(','); //Ищем первую запятую
    if (Pos == -1) //Запятая не найдена - ошибка
    {
        ISMessageBox::ShowCritical(this, LANG("Message.Error.InvalidListValue"));
        return;
    }

    //Запятая найдена - устанавливаем ID и имя объекта на кнопке
    ID = String.mid(0, Pos);
    ButtonMain->setText(String.mid(Pos + 1, String.size() - Pos - 1));

    if (ActionEdit)
    {
        ActionEdit->setEnabled(true);
    }
}
//-----------------------------------------------------------------------------
QVariant ISListEdit::GetValue() const
{
    return ID;
}
//-----------------------------------------------------------------------------
void ISListEdit::Clear()
{
    ID.clear();
    ButtonMain->setText(LANG("NotSelected"));
    ValueChanged();

    if (ActionEdit)
    {
        ActionEdit->setEnabled(false);
    }
}
//-----------------------------------------------------------------------------
void ISListEdit::SetReadOnly(bool read_only)
{
    ISFieldEditBase::SetReadOnly(read_only);
    ButtonMain->setCheckable(!read_only);
    ButtonMain->setToolTip(read_only ? QString() : LANG("ClickFromShowList"));
    ButtonMain->setIcon(read_only ? QIcon() : BUFFER_ICONS("ArrowDown"));
    ButtonMain->setCursor(read_only ? CURSOR_ARROW : CURSOR_POINTING_HAND);
    ButtonList->setVisible(!read_only);
    read_only ?
        disconnect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup) :
        connect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup);
}
//-----------------------------------------------------------------------------
void ISListEdit::InvokeList(PMetaForeign *meta_foreign)
{
    MetaForeign = meta_foreign;
    MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);

    ListEditPopup = new ISListEditPopup(MetaForeign, this);
    connect(ListEditPopup, &ISListEditPopup::Selected, this, &ISListEdit::SelectedValue);
    connect(ListEditPopup, &ISListEditPopup::Created, this, &ISListEdit::SetValue);
    connect(ListEditPopup, &ISListEditPopup::Hided, this, &ISListEdit::HidedPopup);

    if (MetaTable->ShowOnly)
    {
        ButtonList->setPopupMode(QToolButton::DelayedPopup);
        ActionCreate = nullptr;
        ActionEdit = nullptr;
    }
    else
    {
        QMenu *Menu = new QMenu(ButtonList);
        ButtonList->setMenu(Menu);
        ButtonList->setPopupMode(QToolButton::MenuButtonPopup);

        ActionCreate = ISControls::CreateActionCreate(ButtonList->menu());
        connect(ActionCreate, &QAction::triggered, this, &ISListEdit::CreateObject);
        Menu->addAction(ActionCreate);

        ActionEdit = ISControls::CreateActionEdit(ButtonList->menu());
        ActionEdit->setEnabled(false);
        connect(ActionEdit, &QAction::triggered, this, &ISListEdit::EditObject);
        Menu->addAction(ActionEdit);
    }
}
//-----------------------------------------------------------------------------
QString ISListEdit::GetCurrentText() const
{
    return ButtonMain->text();
}
//-----------------------------------------------------------------------------
void ISListEdit::SetEnabled(bool Enabled)
{
    ButtonMain->setEnabled(Enabled);
    ButtonList->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISListEdit::ShowPopup()
{
    ButtonMain->setIcon(BUFFER_ICONS("ArrowUp"));
    ButtonMain->setChecked(true);

    ListEditPopup->SetCurrentValue(ID);
    ListEditPopup->adjustSize();
    ListEditPopup->resize(ButtonMain->width(), ListEditPopup->height());

    QPoint PointButton = ButtonMain->mapToGlobal(QPoint());
    QRect ScreenGeometry = QDesktopWidget().screenGeometry();

    QPoint PointNew = PointButton;
    PointNew.setY(PointNew.y() + ButtonMain->height() - 1);

    if (PointNew.y() + ListEditPopup->height() > ScreenGeometry.height()) //Если виджет выходит за рамку экрана по высоте (внизу)
    {
        PointNew = PointButton;
        PointNew.setY(PointNew.y() - ListEditPopup->height() - 2);
    }

    if (PointNew.x() + ListEditPopup->width() > ScreenGeometry.width()) //Если виджет выходит за рамку экрана по ширине (справа)
    {
        ListEditPopup->resize(ScreenGeometry.width() - PointButton.x(), ListEditPopup->height());
    }

    ListEditPopup->move(PointNew);
    ListEditPopup->show();
}
//-----------------------------------------------------------------------------
void ISListEdit::SelectedValue(const QVariant &id, const QString &text)
{
    ID = id;
    ButtonMain->setText(text);
    ValueChanged();

    if (ActionEdit)
    {
        ActionEdit->setEnabled(true);
    }
}
//-----------------------------------------------------------------------------
void ISListEdit::HidedPopup()
{
    ButtonMain->setIcon(BUFFER_ICONS("ArrowDown"));
    ButtonMain->setChecked(false);
}
//-----------------------------------------------------------------------------
void ISListEdit::ShowListForm()
{
    ISObjectPair SelectedObject = ISGui::SelectObject(MetaTable->Name, MetaTable->LocalListName, GetValue().toInt());
    if (SelectedObject.first != 0) //Если запись была выбрана
    {
        SetValue(QString("%1,%2").arg(SelectedObject.first).arg(SelectedObject.second));
    }
}
//-----------------------------------------------------------------------------
void ISListEdit::CreateObject()
{
    if (ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
    {
        ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::New, MetaTable->Name);
        connect(ObjectFormBase, &ISObjectFormBase::SavedWithListEdit, this, &ISListEdit::SelectedValue);
        ISGui::ShowObjectForm(ObjectFormBase);
    }
    else
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
    }
}
//-----------------------------------------------------------------------------
void ISListEdit::EditObject()
{
    if (ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->Name, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
    {
        ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, MetaTable->Name, GetValue().toInt());
        connect(ObjectFormBase, &ISObjectFormBase::SavedWithListEdit, this, &ISListEdit::SelectedValue);
        ISGui::ShowObjectForm(ObjectFormBase);
    }
    else
    {
        ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTaskExecutorEdit::ISTaskExecutorEdit(QWidget *parent) : ISListEdit(parent)
{
    ButtonDesignateMe = new ISPushButton(this);
    ButtonDesignateMe->setText(LANG("Task.DesignateMe"));
    ButtonDesignateMe->setIcon(BUFFER_ICONS("User"));
    ButtonDesignateMe->setCursor(CURSOR_POINTING_HAND);
    ButtonDesignateMe->setSizePolicy(QSizePolicy::Maximum, ButtonDesignateMe->sizePolicy().verticalPolicy());
    connect(ButtonDesignateMe, &ISPushButton::clicked, this, &ISTaskExecutorEdit::DesignateMe);
    AddWidgetToRight(ButtonDesignateMe);
}
//-----------------------------------------------------------------------------
ISTaskExecutorEdit::~ISTaskExecutorEdit()
{

}
//-----------------------------------------------------------------------------
void ISTaskExecutorEdit::DesignateMe()
{
    SetValue(CURRENT_USER_ID);
}
//-----------------------------------------------------------------------------
