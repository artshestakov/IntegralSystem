#include "ISInputDialog.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISFieldEdits.h"
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

		//Если добавляется поле редактирования большого текста - не вставляем растяжение
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
