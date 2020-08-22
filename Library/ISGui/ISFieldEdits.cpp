#include "ISFieldEdits.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISFileDialog.h"
#include "ISGui.h"
#include "ISPassword.h"
#include "ISPopupMessage.h"
#include "ISStyleSheet.h"
#include "ISDefinesCore.h"
#include "ISDefinesGui.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISPassportForm.h"
#include "ISSettingsDatabase.h"
#include "ISSettings.h"
//-----------------------------------------------------------------------------
static QString QI_ASTERISK_QUEUE = PREPARE_QUERY("INSERT INTO _asteriskqueue(astq_type, astq_initiated, astq_parameters) "
												 "VALUES((SELECT asqt_id FROM _asteriskqueuetype WHERE asqt_uid = :TypeUID), currentuserid(), :Parameters)");
//-----------------------------------------------------------------------------
static QString QS_SEARCH_FAST = PREPARE_QUERY("SELECT srfs_value "
											  "FROM _searchfast "
											  "WHERE srfs_user = currentuserid() "
											  "ORDER BY srfs_id "
											  "LIMIT :Limit");
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
	ISLineEdit::SetValue(ISUuid(value.toString()));
}
//-----------------------------------------------------------------------------
bool ISUuidEdit::IsValid() const
{
	return !ISUuid(GetValue().toString()).isEmpty();
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
	SetValue(ISSystem::GenerateUuid());
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
	return ISDateTimeEdit::GetValue().toDate();
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISWeekDayEdit::ISWeekDayEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);

	AddItem(LANG("Everyday"), 0);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Monday), Qt::Monday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Tuesday), Qt::Tuesday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Wednesday), Qt::Wednesday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Thursday), Qt::Thursday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Friday), Qt::Friday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Saturday), Qt::Saturday);
	AddItem(ISSystem::GetDayOfWeekName(Qt::Sunday), Qt::Sunday);
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
	SetVisibleClear(true); //„тобы кнопка очистки отображалась, пор€док SetReadOnly и SetVisibleClear доллжен быть именно такой
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
	//„тобы кнопка очистки отображалась, пор€док SetReadOnly и SetVisibleClear доллжен быть именно такой
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
ISPasswordEdit::ISPasswordEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetEchoMode(QLineEdit::Password);
	SetPlaceholderText(LANG("Field.Password.PlaceholderText"));

	CheckPasswordVisible = new QCheckBox(this);
	CheckPasswordVisible->setFocusPolicy(Qt::NoFocus);
	CheckPasswordVisible->setCursor(CURSOR_POINTING_HAND);
	CheckPasswordVisible->setToolTip(LANG("ShowHidePassword"));
	connect(CheckPasswordVisible, &QCheckBox::stateChanged, this, &ISPasswordEdit::PasswordVisibleChanged);
	AddWidgetToLeft(CheckPasswordVisible);

	ButtonGeneratePassword = new ISServiceButton(BUFFER_ICONS("PasswordGenerate"), LANG("GeneratePassword"), this);
	ButtonGeneratePassword->setVisible(true);
	connect(ButtonGeneratePassword, &ISPushButton::clicked, this, &ISPasswordEdit::GeneratePassword);
	AddWidgetToRight(ButtonGeneratePassword);

	SetVisibleGenerate(false);
}
//-----------------------------------------------------------------------------
ISPasswordEdit::~ISPasswordEdit()
{

}
//-----------------------------------------------------------------------------
void ISPasswordEdit::SetVisibleGenerate(bool Visible)
{
	ButtonGeneratePassword->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISPasswordEdit::SetVisibleCheckBox(bool Visible)
{
	CheckPasswordVisible->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISPasswordEdit::PasswordVisibleChanged(int State)
{
	switch (State)
	{
	case Qt::Checked: SetEchoMode(QLineEdit::Normal); break;
	case Qt::Unchecked: SetEchoMode(QLineEdit::Password); break;
	}
	SetFocus();
}
//-----------------------------------------------------------------------------
void ISPasswordEdit::GeneratePassword()
{
	SetValue(ISPassword::GeneratePassword());
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
	QRadioButton *ButtonLow = new QRadioButton(this);
	ButtonLow->setText(LANG("Task.Priority.Low"));
	ButtonLow->setToolTip(LANG("Task.Priority.Low.ToolTip"));
	ButtonLow->setStyleSheet(STYLE_SHEET("Task.Priority.Low"));
	ButtonLow->setIcon(BUFFER_ICONS("Task.Priority.Low"));
	ButtonLow->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	AddButton(ButtonLow, 1);

	QRadioButton *ButtonAverage = new QRadioButton(this);
	ButtonAverage->setText(LANG("Task.Priority.Average"));
	ButtonAverage->setToolTip(LANG("Task.Priority.Average.ToolTip"));
	ButtonAverage->setStyleSheet(STYLE_SHEET("Task.Priority.Average"));
	ButtonAverage->setIcon(BUFFER_ICONS("Task.Priority.Average"));
	ButtonAverage->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	AddButton(ButtonAverage, 2);

	QRadioButton *ButtonTall = new QRadioButton(this);
	ButtonTall->setText(LANG("Task.Priority.Tall"));
	ButtonTall->setToolTip(LANG("Task.Priority.Tall.ToolTip"));
	ButtonTall->setStyleSheet(STYLE_SHEET("Task.Priority.Tall"));
	ButtonTall->setIcon(BUFFER_ICONS("Task.Priority.Tall"));
	ButtonTall->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	AddButton(ButtonTall, 3);

	//”становка низкого приоритета задачи по умолчанию
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
	GetCheckBox()->setIcon(BUFFER_ICONS(value.toBool() ? "Task.Important.Checked" : "Task.Important.Unchecked"));
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
	return ISDateTimeEdit::GetValue().toTime();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDoubleEdit::ISDoubleEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
}
//-----------------------------------------------------------------------------
ISDoubleEdit::~ISDoubleEdit()
{

}
//-----------------------------------------------------------------------------
void ISDoubleEdit::SetValue(const QVariant &value)
{
	bool Ok = true;
	double Double = value.toDouble(&Ok);
	Ok ? ISLineEdit::SetValue(QString::fromStdString(ISAlgorithm::PrepareDouble(Double, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA)))) :
		ISLineEdit::SetValue(QString());
}
//-----------------------------------------------------------------------------
void ISDoubleEdit::TextChanged(const QString &Text)
{
	QString String = Text;
	ISAlgorithm::PrepareStringDouble(String, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA));
	if (String != Text)
	{
		ISLineEdit::TextChangedDisconnect();
		ISLineEdit::SetValue(String);
		ISLineEdit::TextChangedConnect();
	}
	ISLineEdit::TextChanged(String);
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
ISExecutorEdit::ISExecutorEdit(QWidget *parent) : ISListEdit(parent)
{
	ButtonDesignateMe = new ISPushButton(this);
	ButtonDesignateMe->setText(LANG("Task.DesignateMe"));
	ButtonDesignateMe->setIcon(BUFFER_ICONS("User"));
	ButtonDesignateMe->setCursor(CURSOR_POINTING_HAND);
	ButtonDesignateMe->setSizePolicy(QSizePolicy::Maximum, ButtonDesignateMe->sizePolicy().verticalPolicy());
	connect(ButtonDesignateMe, &ISPushButton::clicked, this, &ISExecutorEdit::DesignateMe);
	AddWidgetToRight(ButtonDesignateMe);
}
//-----------------------------------------------------------------------------
ISExecutorEdit::~ISExecutorEdit()
{

}
//-----------------------------------------------------------------------------
void ISExecutorEdit::SetReadOnly(bool read_only)
{
	ISListEdit::SetReadOnly(read_only);
	ButtonDesignateMe->setVisible(!read_only);
}
//-----------------------------------------------------------------------------
void ISExecutorEdit::DesignateMe()
{
	SetValue(CURRENT_USER_ID);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISIntegerEdit::ISIntegerEdit(QWidget *parent)
	: ISLineEdit(parent),
	IntValidator(new QIntValidator(this))
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
	SetRange(Minimum, INT_MAX);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetMaximum(int Maximum)
{
	SetRange(INT_MIN, Maximum);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::SetRange(int Minimum, int Maximum)
{
	if (IntValidator)
	{
		delete IntValidator;
	}

	IntValidator = new QIntValidator(Minimum, Maximum, this);
	SetValidator(IntValidator);
}
//-----------------------------------------------------------------------------
void ISIntegerEdit::ResetRange()
{
	SetRange(INT_MIN, INT_MAX);
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
ISPassportEdit::ISPassportEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetReadOnly(true);
	Clear();

	ISServiceButton *ButtonEdit = new ISServiceButton(BUFFER_ICONS("Passport"), LANG("Passport.EditPassport"), this);
	ButtonEdit->setFocusPolicy(Qt::NoFocus);
	connect(ButtonEdit, &ISServiceButton::clicked, this, &ISPassportEdit::Edit);
	AddWidgetToRight(ButtonEdit);
}
//-----------------------------------------------------------------------------
ISPassportEdit::~ISPassportEdit()
{

}
//-----------------------------------------------------------------------------
void ISPassportEdit::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		ISLineEdit::SetValue(PreparePassport(value.toString()));
	}
	else
	{
		GetLineEdit()->setText(LANG("NotFill"));
	}

	PassportString = value.toString();
}
//-----------------------------------------------------------------------------
QVariant ISPassportEdit::GetValue() const
{
	return PassportString;
}
//-----------------------------------------------------------------------------
void ISPassportEdit::Clear()
{
	disconnect(GetLineEdit(), &ISQLineEdit::textChanged, this, &ISPassportEdit::ValueChanged);
	GetLineEdit()->setText(LANG("NotFill"));
	connect(GetLineEdit(), &ISQLineEdit::textChanged, this, &ISPassportEdit::ValueChanged);

	PassportString.clear();
}
//-----------------------------------------------------------------------------
void ISPassportEdit::Edit()
{
	ISGui::SetWaitGlobalCursor(true);
	ISPassportForm PassportForm(PassportString);
	ISGui::SetWaitGlobalCursor(false);
	if (PassportForm.Exec())
	{
		PassportString = PassportForm.GetPassportString();
		SetValue(PassportString);
		SetFocus();
	}
}
//-----------------------------------------------------------------------------
QString ISPassportEdit::PreparePassport(const QString &passport_string)
{
	QString Result;
	if (!passport_string.isEmpty())
	{
		QStringList StringList = passport_string.split(ISDefines::Core::SYMBOL_SPACE_HIDE);
		QString Seria = StringList.at(0);
		QString Number = StringList.at(1);
		QString DateOfIssue = StringList.at(2);
		QString Issued = StringList.at(3);
		QString DivisionCode = StringList.at(4);

		if (Seria.length())
		{
			Result += LANG("Passport.Seria") + ": " + Seria + SYMBOL_SPACE;
		}

		if (Number.length())
		{
			Result += LANG("Passport.Number") + ": " + Number + SYMBOL_SPACE;
		}

		if (DateOfIssue.length())
		{
			Result += LANG("Passport.DateOfIssue") + ": " + DateOfIssue + SYMBOL_SPACE;
		}

		if (Issued.length())
		{
			Result += LANG("Passport.Issued") + ": " + Issued + SYMBOL_SPACE;
		}

		if (DivisionCode.length())
		{
			Result += LANG("Passport.DivisionCode") + ": " + DivisionCode + SYMBOL_SPACE;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISPhoneEdit::ISPhoneEdit(QWidget *parent) : ISPhoneBaseEdit(parent)
{
	SetVisibleCall(true);
	if (!ISGui::CheckSetupTelephony())
	{
		SetToolTipCall(LANG("NotSettingTelephonyForCurrentUser"));
		SetCursorCall(CURSOR_FORBIDDEN);
	}
}
//-----------------------------------------------------------------------------
ISPhoneEdit::~ISPhoneEdit()
{

}
//-----------------------------------------------------------------------------
void ISPhoneEdit::Call()
{
	if (ISGui::CheckSetupTelephony())
	{
		ISGui::SetWaitGlobalCursor(true);
		QString Phone = GetValue().toString();

		QVariantMap VariantMap;
		VariantMap.insert("Phone", Phone);

		ISQuery qInsert(QI_ASTERISK_QUEUE);
		qInsert.BindValue(":TypeUID", CONST_UID_ASTERISK_QUEUE_TYPE_OUT_CALLED);
		qInsert.BindValue(":Parameters", ISSystem::VariantMapToJsonString(VariantMap));
		if (qInsert.Execute())
		{
			ISPopupMessage::ShowNotification(LANG("OutcomingCall").arg(Phone));
			emit Called();
		}
		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		ISMessageBox::ShowInformation(this, LANG("NotSettingTelephonyForCurrentUser"));
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSearchEdit::ISSearchEdit(QWidget *parent) : ISLineEdit(parent)
{
	connect(this, &ISSearchEdit::DataChanged, this, &ISSearchEdit::SearchChanged);

	SetPlaceholderText(LANG("Search"));
	SetToolTip(LANG("EnteringSearchQuery"));
	SetIcon(BUFFER_ICONS("Search"));

	if (SETTING_BOOL(CONST_UID_SETTING_TABLES_SEARCH_FAST_REMEMBER))
	{
		ButtonLastSearch = new ISServiceButton(LANG("ClickFromViewMyLLastSearchQuery"), this);
		ButtonLastSearch->setText("...");
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
		delete ButtonLastSearch->menu()->actions().takeAt(0);
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
	SetValue(dynamic_cast<QAction*>(sender())->text());
}
//-----------------------------------------------------------------------------
void ISSearchEdit::Timeout()
{
	Timer->stop();
	ISGui::RepaintWidget(this);
	emit Search(GetValue().toString());
}
//-----------------------------------------------------------------------------
