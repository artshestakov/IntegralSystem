#include "ISInputDialog.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISIntegerEdit.h"
#include "ISComboEdit.h"
#include "ISGui.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
ISInputDialog::ISInputDialog(ISNamespace::FieldType DataType, const QString &Title, const QString &LabelText, const QVariant &Value)
	: ISInterfaceDialogForm(),
	FieldEditBase(nullptr),
	Label(nullptr)
{
	setWindowTitle(Title);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	ForbidResize();

	if (!LabelText.isEmpty())
	{
		Label = new QLabel(this);
		Label->setText(LabelText);
		Label->setWordWrap(true);
		GetMainLayout()->addWidget(Label, 0, Qt::AlignLeft);
	}

	if (DataType != ISNamespace::FT_Unknown)
	{
		FieldEditBase = ISGui::CreateColumnForField(this, DataType);
		FieldEditBase->SetValue(Value);
		GetMainLayout()->addWidget(FieldEditBase);
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
int ISInputDialog::GetInteger(const QString &Title, const QString &LabelText, int Minimum, int Maximum, const QVariant &Value)
{
	ISInputDialog InputDialog(ISNamespace::FT_Int, Title, LabelText, Value);
	ISIntegerEdit *IntegerEdit = dynamic_cast<ISIntegerEdit*>(InputDialog.GetEditWidget());
	IntegerEdit->SetRange(Minimum, Maximum);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toInt();
	}
	return NPOS;
}
//-----------------------------------------------------------------------------
QDateTime ISInputDialog::GetDateTime(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(ISNamespace::FT_DateTime, Title, LabelText);
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
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toTime();
	}
	return QTime();
}
//-----------------------------------------------------------------------------
QString ISInputDialog::GetPassword(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(ISNamespace::FT_String, Title, LabelText);
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
void ISInputDialog::Apply()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISInputDialog::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	FieldEditBase->SetFocus();
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
