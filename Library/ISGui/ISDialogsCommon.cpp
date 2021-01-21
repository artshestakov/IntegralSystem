#include "ISDialogsCommon.h"
#include "ISProperty.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISFieldEdits.h"
#include "ISGui.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
ISFileDialog::ISFileDialog(QWidget *parent) : QFileDialog(parent)
{

}
//-----------------------------------------------------------------------------
ISFileDialog::~ISFileDialog()
{

}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetOpenFileNameImage(QWidget *parent)
{
	QString FilterString;
	FilterString += LANG("File.Filter.Images") + ";;";
	FilterString += LANG("File.Filter.Image.PNG") + ";;";
	FilterString += LANG("File.Filter.Image.JPEG") + ";;";
	FilterString += LANG("File.Filter.Image.JPG") + ";;";
	FilterString += LANG("File.Filter.Image.DDS") + ";;";
	FilterString += LANG("File.Filter.Image.GIF") + ";;";
	FilterString += LANG("File.Filter.Image.ICNS") + ";;";
	FilterString += LANG("File.Filter.Image.ICO") + ";;";
	FilterString += LANG("File.Filter.Image.SVG") + ";;";
	FilterString += LANG("File.Filter.Image.TGA") + ";;";
	FilterString += LANG("File.Filter.Image.TIFF") + ";;";
	FilterString += LANG("File.Filter.Image.WBMP") + ";;";
	FilterString += LANG("File.Filter.Image.BMP") + ";;";
	FilterString += LANG("File.Filter.Image.WEBP");

	QString Path = ISFileDialog::getOpenFileName(parent, LANG("FileDialog.Image.Select.Title"), PROPERTY_GET("PathLastDirectory").toString(), FilterString);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileNameImage(QWidget *parent, const QString &FileName)
{
	QString FilterString;
	FilterString += LANG("File.Filter.Images") + ";;";
	FilterString += LANG("File.Filter.Image.PNG") + ";;";
	FilterString += LANG("File.Filter.Image.JPEG") + ";;";
	FilterString += LANG("File.Filter.Image.JPG") + ";;";
	FilterString += LANG("File.Filter.Image.DDS") + ";;";
	FilterString += LANG("File.Filter.Image.GIF") + ";;";
	FilterString += LANG("File.Filter.Image.ICNS") + ";;";
	FilterString += LANG("File.Filter.Image.ICO") + ";;";
	FilterString += LANG("File.Filter.Image.SVG") + ";;";
	FilterString += LANG("File.Filter.Image.TGA") + ";;";
	FilterString += LANG("File.Filter.Image.TIFF") + ";;";
	FilterString += LANG("File.Filter.Image.WBMP") + ";;";
	FilterString += LANG("File.Filter.Image.BMP") + ";;";
	FilterString += LANG("File.Filter.Image.WEBP");

	QString Path = ISFileDialog::getSaveFileName(parent, LANG("FileDialog.Image.Save.Title"), PROPERTY_GET("PathLastDirectory").toString() + (FileName.isEmpty() ? QString() : '/' + FileName), FilterString);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetOpenFileName(QWidget *parent, const QString &FilePath, const QString &FilterName)
{
	QString Path = ISFileDialog::getOpenFileName(parent, LANG("FileDialog.File.Open.Title"), FilePath.isEmpty() ? PROPERTY_GET("PathLastDirectory").toString() : FilePath, FilterName);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName)
{
	QString Path = ISFileDialog::getSaveFileName(parent, LANG("FileDialog.File.Save.Title"), PROPERTY_GET("PathLastDirectory").toString() + (FileName.isEmpty() ? QString() : '/' + FileName), FilterName);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QString ISFileDialog::GetDirectoryPath(QWidget *parent)
{
	QString Path = ISFileDialog::getExistingDirectory(parent, LANG("FileDialog.Directory.Title"), PROPERTY_GET("PathLastDirectory").toString(), ISFileDialog::ShowDirsOnly);
	if (!Path.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(Path).dir().path());
	}
	return Path;
}
//-----------------------------------------------------------------------------
QStringList ISFileDialog::GetOpenFileNames(QWidget *parent, const QString &FilterName)
{
	QStringList StringList = ISFileDialog::getOpenFileNames(parent, LANG("FileDialog.Files.Open.Title"), PROPERTY_GET("PathLastDirectory").toString(), FilterName);
	if (!StringList.isEmpty())
	{
		PROPERTY_SET("PathLastDirectory", QFileInfo(StringList.front()).dir().path());
	}
	return StringList;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISInputDialog::ISInputDialog(ISNamespace::FieldType DataType, const QString &Title, const QString &LabelText, const QVariant &Value)
	: ISInterfaceDialogForm(),
	FieldEditBase(nullptr),
	Label(nullptr)
{
	setWindowTitle(Title);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	if (!LabelText.isEmpty())
	{
		QHBoxLayout *LayoutLabel = new QHBoxLayout();
		GetMainLayout()->addLayout(LayoutLabel);

		Label = new QLabel(LabelText, this);
		Label->setWordWrap(true);
		Label->setSizePolicy(QSizePolicy::Minimum, Label->sizePolicy().verticalPolicy());
		LayoutLabel->addWidget(Label);
	}

	if (DataType != ISNamespace::FT_Unknown)
	{
		FieldEditBase = ISGui::CreateColumnForField(this, DataType);
		FieldEditBase->SetValue(Value);
		GetMainLayout()->addWidget(FieldEditBase);

		//���� ����������� ���� �������������� �������� ������ - �� ��������� ����������
		if (DataType == ISNamespace::FT_Text)
		{
			FieldEditBase->SetSizePolicyVertical(QSizePolicy::Minimum);
		}
		else
		{
			FieldEditBase->SetSizePolicyHorizontal(QSizePolicy::Minimum);
			GetMainLayout()->addStretch();
		}
	}

	ISButtonDialog *ButtonDialog = new ISButtonDialog(this, "OK", LANG("Cancel"));
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISInputDialog::Apply);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISInputDialog::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISInputDialog::~ISInputDialog()
{

}
//-----------------------------------------------------------------------------
QString ISInputDialog::GetString(const QString &Title, const QString &LabelText, const QVariant &Value)
{
	ISInputDialog InputDialog(ISNamespace::FT_String, Title, LabelText, Value);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISInputDialog::GetText(const QString &Title, const QString &LabelText, const QVariant &Value)
{
	ISInputDialog InputDialog(ISNamespace::FT_Text, Title, LabelText, Value);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISInputDialog::GetText(const QString &Title, const QString &LabelText, const QVariant &Value, bool &Ok)
{
	ISInputDialog InputDialog(ISNamespace::FT_Text, Title, LabelText, Value);
	Ok = InputDialog.Exec();
	if (Ok)
	{
		return InputDialog.GetValue().toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
int ISInputDialog::GetInteger(bool &Ok, const QString &Title, const QString &LabelText, int Minimum, int Maximum, const QVariant &Value)
{
	ISInputDialog InputDialog(ISNamespace::FT_Int, Title, LabelText, Value);
	InputDialog.ForbidResize();
	dynamic_cast<ISIntegerEdit*>(InputDialog.GetEditWidget())->SetRange(Minimum, Maximum);
	Ok = InputDialog.Exec();
	if (Ok)
	{
		return InputDialog.GetValue().toInt();
	}
	return 0;
}
//-----------------------------------------------------------------------------
unsigned int ISInputDialog::GetUInteger(bool &Ok, const QString &Title, const QString &LabelText, unsigned int Maximum, const QVariant &Value)
{
	return (unsigned int)ISInputDialog::GetInteger(Ok, Title, LabelText, 0, Maximum, Value);
}
//-----------------------------------------------------------------------------
QDateTime ISInputDialog::GetDateTime(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(ISNamespace::FT_DateTime, Title, LabelText);
	InputDialog.ForbidResize();
	if (InputDialog.Exec())
	{
		QDateTime DateTime = InputDialog.GetValue().toDateTime();
		DateTime.setTime(QTime(DateTime.time().hour(), DateTime.time().minute()));
		return DateTime;
	}
	return QDateTime();
}
//-----------------------------------------------------------------------------
QDate ISInputDialog::GetDate(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(ISNamespace::FT_Date, Title, LabelText);
	InputDialog.ForbidResize();
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toDate();
	}
	return QDate();
}
//-----------------------------------------------------------------------------
QTime ISInputDialog::GetTime(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(ISNamespace::FT_Time, Title, LabelText);
	InputDialog.ForbidResize();
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toTime();
	}
	return QTime();
}
//-----------------------------------------------------------------------------
QString ISInputDialog::GetPassword()
{
	ISInputDialog InputDialog(ISNamespace::FT_Password, LANG("Password"), LANG("EnterThePassword") + ':');
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetList(const QString &Title, const QString &LabelText, const ISStringToVariantMap &VariantMap)
{
	ISInputDialog InputDialog(ISNamespace::FT_Unknown, Title, LabelText);
	ISComboEdit *ComboEdit = new ISComboEdit(&InputDialog);
	ComboEdit->setSizePolicy(QSizePolicy::Minimum, ComboEdit->sizePolicy().verticalPolicy());
	ComboEdit->SetEditable(false);
	for (const auto &MapItem : VariantMap)
	{
		ComboEdit->AddItem(MapItem.first, MapItem.second);
	}
	InputDialog.SetEditWidget(ComboEdit);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetValue() const
{
	return FieldEditBase->GetValue();
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISInputDialog::GetEditWidget()
{
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISInputDialog::SetEditWidget(ISFieldEditBase *field_edit_base)
{
	FieldEditBase = field_edit_base;
	GetMainLayout()->insertWidget(GetMainLayout()->indexOf(Label) + 1, FieldEditBase);
}
//-----------------------------------------------------------------------------
void ISInputDialog::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	FieldEditBase->SetFocus();
}
//-----------------------------------------------------------------------------
void ISInputDialog::Apply()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISInputDialog::EnterClicked()
{
	Apply();
}
//-----------------------------------------------------------------------------
void ISInputDialog::ControlEnterClicked()
{
	Apply();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISMessageBox::ISMessageBox(ISMessageBox::Icon Icon, const QString &Title, const QString &Message, const QString &DetailedText, const std::vector<ISMessageBoxButton> &Buttons, QWidget *parent)
	: QMessageBox(Icon, Title, Message, ISMessageBox::NoButton, parent),
	ClickedID(-1)
{
	setWindowIcon(BUFFER_ICONS("Logo"));
	setTextFormat(Qt::PlainText);
	setTextInteractionFlags(Qt::TextSelectableByMouse);
	AddButtons(Buttons);
	setText(DetailedText.isEmpty() ? Message : Message + "\n\n" + DetailedText);
}
//-----------------------------------------------------------------------------
ISMessageBox::~ISMessageBox()
{

}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowInformation(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox(ISMessageBox::Information, LANG("Information"), Message, DetailedText, { { 1, "OK", true } }, parent).Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowWarning(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox(ISMessageBox::Warning, LANG("Warning"), Message, DetailedText, { { 1, "OK", true } }, parent).Exec();
}
//-----------------------------------------------------------------------------
void ISMessageBox::ShowCritical(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	ISMessageBox(ISMessageBox::Critical, LANG("Error"), Message, DetailedText, { { 1, "OK", true } }, parent).Exec();
}
//-----------------------------------------------------------------------------
bool ISMessageBox::ShowQuestion(QWidget *parent, const QString &Message, const QString &DetailedText)
{
	return ISMessageBox(ISMessageBox::Question, LANG("Question"), Message, DetailedText,
	{
		{ 1, LANG("Yes") },
		{ 2, LANG("No"), true }
	}, parent).Exec() == 1;
}
//-----------------------------------------------------------------------------
int ISMessageBox::ShowQuestion(QWidget *parent, const QString &Message, const std::vector<ISMessageBoxButton> &Buttons, const QString &DetailedText)
{
	return ISMessageBox(ISMessageBox::Question, LANG("Question"), Message, DetailedText, Buttons, parent).Exec();
}
//-----------------------------------------------------------------------------
int ISMessageBox::Exec()
{
	QApplication::beep();
	PROCESS_EVENTS();
	for (QAbstractButton *AbstractButton : buttons())
	{
		AbstractButton->setStyleSheet(BUFFER_STYLE_SHEET("ISPushButton"));
		AbstractButton->setFixedHeight(ISPUSHBUTTON_MINIMUM_HEIGHT);
	}
	exec();
	return ClickedID;
}
//-----------------------------------------------------------------------------
void ISMessageBox::AddButtons(const std::vector<ISMessageBoxButton> &Buttons)
{
	for (const ISMessageBoxButton &Button : Buttons)
	{
		ISPushButton *PushButton = new ISPushButton(Button.Icon, Button.Text, this);
		PushButton->setProperty("ID", Button.ID);
		connect(PushButton, &ISPushButton::clicked, this, &ISMessageBox::ButtonClicked);
		addButton(PushButton, ISMessageBox::ActionRole);
		if (Button.Default)
		{
			PushButton->setFocus();
		}
	}
}
//-----------------------------------------------------------------------------
void ISMessageBox::SetDefaultButton(int ID)
{
	for (QAbstractButton *AbstractButton : buttons())
	{
		if (AbstractButton->property("ID").toInt() == ID)
		{
			AbstractButton->setFocus();
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISMessageBox::ButtonClicked()
{
	ClickedID = sender()->property("ID").toInt();
}
//-----------------------------------------------------------------------------
