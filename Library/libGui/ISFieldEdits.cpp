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
//-----------------------------------------------------------------------------
ISBIKEdit::ISBIKEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_9), this));
	SetPlaceholderText(LANG("Field.Bik.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISBIKEdit::~ISBIKEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISKPPEdit::ISKPPEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_9), this));
	SetPlaceholderText(LANG("Field.Kpp.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISKPPEdit::~ISKPPEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOGRNEdit::ISOGRNEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_13), this));
	SetPlaceholderText(LANG("Field.Ogrn.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISOGRNEdit::~ISOGRNEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISOKPOEdit::ISOKPOEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_8), this));
	SetPlaceholderText(LANG("Field.Okpo.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISOKPOEdit::~ISOKPOEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISPercentEdit::ISPercentEdit(QWidget *parent) : ISDoubleEdit(parent)
{
	SetRange(0, 100);
}
//-----------------------------------------------------------------------------
ISPercentEdit::~ISPercentEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISUuidEdit::ISUuidEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetUppercase(true);
	SetPlaceholderText("{00000000-0000-0000-0000-000000000000}");

	ISServiceButton *ButtonGenerate = new ISServiceButton(this);
	ButtonGenerate->setToolTip(LANG("ClickFromGenerateNewUID"));
	ButtonGenerate->setIcon(BUFFER_ICONS("UUID.Generate"));
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISVINEdit::ISVINEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_17), this));
	SetPlaceholderText(LANG("Field.Vin.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISVINEdit::~ISVINEdit()
{

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
	Label->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	Label->setSizePolicy(QSizePolicy::Maximum, Label->sizePolicy().verticalPolicy());
	AddWidgetToRight(Label);

	UpdateLabel(QDate());

	connect(this, &ISBirthdayEdit::DataChanged, [=]
	{
		UpdateLabel(GetValue().toDate());
	});
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
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetPlaceholderText(LANG("Field.Inn.PlaceholderText"));
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_12), this));

	ButtonSearch = new ISServiceButton(this);
	ButtonSearch->setVisible(false);
	ButtonSearch->setToolTip(LANG("SearchFromINN"));
	ButtonSearch->setIcon(BUFFER_ICONS("Taxation"));
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
	ButtonSelect->setText(LANG("Overview") + "...");
	ButtonSelect->setToolTip(LANG("SelectFolder"));
	connect(ButtonSelect, &ISPushButton::clicked, this, &ISPathEditDir::SelectDir);
	AddWidgetToRight(ButtonSelect);

	ButtonOpenDir = new ISServiceButton(this);
	ButtonOpenDir->setIcon(BUFFER_ICONS("FolderClosed"));
	ButtonOpenDir->setToolTip(LANG("OpenFolder"));
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

	if (value.toString().length())
	{
		ButtonOpenDir->setEnabled(true);
	}
	else
	{
		ButtonOpenDir->setEnabled(false);
	}
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
	if (FolderPath.length())
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
	ButtonSelect->setText(LANG("Overview") + "...");
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
	if (Path.length())
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

	ButtonGeneratePassword = new ISServiceButton(this);
	ButtonGeneratePassword->setVisible(true);
	ButtonGeneratePassword->setToolTip(LANG("GeneratePassword") + "...");
	ButtonGeneratePassword->setIcon(BUFFER_ICONS("PasswordGenerate"));
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

	ButtonUrl = new ISServiceButton(this);
	ButtonUrl->setToolTip(LANG("OpenUrlLink"));
	ButtonUrl->setIcon(BUFFER_ICONS("Url"));
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
	if (UrlString.length())
	{
		if (ISGui::OpenUrl(UrlString))
		{
			ISPopupMessage *PopupMessage = new ISPopupMessage(nullptr);
			PopupMessage->SetTitle(LANG("OpeningUrl") + "...");
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
	if (GetValue().toString().length())
	{
		ButtonUrl->setEnabled(true);
	}
	else
	{
		ButtonUrl->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISYearEdit::ISYearEdit(QWidget *parent) : ISIntegerEdit(parent)
{
	SetMinimum(YEAR_MINIMUM);
	SetMaximum(YEAR_MAXIMUM);

	ISServiceButton *ButtonCurrentYear = new ISServiceButton(this);
	ButtonCurrentYear->setToolTip(LANG("CurrentYear"));
	ButtonCurrentYear->setIcon(BUFFER_ICONS("Calendar"));
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
ISMonthEdit::ISMonthEdit(QWidget *parent) : ISComboEdit(parent)
{
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
	setMaximumWidth(280);

	SetIcon(BUFFER_ICONS("EMail.FieldEdit.Acceptable"));
	SetPlaceholderText(LANG("Field.EMail.PlaceholderText"));

	ISServiceButton *ButtonSend = new ISServiceButton(this);
	ButtonSend->setToolTip(LANG("Send"));
	ButtonSend->setIcon(BUFFER_ICONS("EMailSend"));
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
	if (EMail.length())
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
	connect(GetLineEdit(), &ISQLineEdit::FocusInSignal, this, &ISDivisionCodeEdit::SelectAll);
}
//-----------------------------------------------------------------------------
ISDivisionCodeEdit::~ISDivisionCodeEdit()
{

}
//-----------------------------------------------------------------------------
bool ISDivisionCodeEdit::IsValid() const
{
	return GetValue().toString().length() == 7;
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
ISTaskNameEdit::ISTaskNameEdit(QWidget *parent) : ISLineEdit(parent)
{
	Completer = nullptr;

	QFile File(ISDefines::Core::PATH_TEMP_DIR + "/TaskNameLog");
	if (File.exists())
	{
		if (File.open(QIODevice::ReadOnly))
		{
			QString String = File.readAll();
			StringList = String.split("\n");
			StringList.removeOne(QString());
			File.close();

			CreateCompleter(StringList);
		}
	}
}
//-----------------------------------------------------------------------------
ISTaskNameEdit::~ISTaskNameEdit()
{

}
//-----------------------------------------------------------------------------
void ISTaskNameEdit::Invoke()
{
	QString TaskName = GetValue().toString();
	if (!StringList.contains(TaskName))
	{
		StringList.append(TaskName);
	}

	StringList.removeDuplicates();
	StringList.sort();

	QFile File(ISDefines::Core::PATH_TEMP_DIR + "/TaskNameLog");
	if (File.open(QIODevice::Truncate | QIODevice::Append))
	{
		QTextStream TextStream(&File);
		for (const QString &String : StringList)
		{
			TextStream << String << endl;
		}
		File.close();
	}

	CreateCompleter(StringList);
}
//-----------------------------------------------------------------------------
void ISTaskNameEdit::CreateCompleter(const QStringList &StringList)
{
	if (Completer)
	{
		delete Completer;
		Completer = nullptr;
	}

	Completer = new QCompleter(StringList, this);
	Completer->setMaxVisibleItems(20);
	Completer->setFilterMode(Qt::MatchContains);
	Completer->setCaseSensitivity(Qt::CaseInsensitive);
	Completer->setCompletionMode(QCompleter::PopupCompletion);
	SetCompleter(Completer);
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
	if (value.toBool())
	{
		GetCheckBox()->setIcon(BUFFER_ICONS("Task.Important.Checked"));
	}
	else
	{
		GetCheckBox()->setIcon(BUFFER_ICONS("Task.Important.Unchecked"));
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTaskDescriptionEdit::ISTaskDescriptionEdit(QWidget *parent) : ISTextEdit(parent)
{
	GetTextEdit()->setMaximumHeight(75);
}
//-----------------------------------------------------------------------------
ISTaskDescriptionEdit::~ISTaskDescriptionEdit()
{

}
//-----------------------------------------------------------------------------
