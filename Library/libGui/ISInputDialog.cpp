#include "ISInputDialog.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISIntegerEdit.h"
#include "ISButtons.h"
#include "ISComboEdit.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISInputDialog::ISInputDialog(const QString &Title, const QString &LabelText)
	: ISInterfaceDialogForm(),
	AddingFieldEdit(false),
	FieldEditBase(nullptr),
	Label(nullptr)
{
	setWindowTitle(Title);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	if (!LabelText.isEmpty())
	{
		Label = new QLabel(this);
		Label->setText(LabelText);
		Label->setWordWrap(true);
		GetMainLayout()->addWidget(Label);
	}

	LayoutField = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutField);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->addStretch();
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonApply = new ISPushButton(this);
	ButtonApply->setText("OK");
	ButtonApply->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonApply, &ISPushButton::clicked, this, &ISInputDialog::Apply);
	LayoutBottom->addWidget(ButtonApply);

	ISPushButton *ButtonCancel = new ISPushButton(this);
	ButtonCancel->setText(LANG("Cancel"));
	ButtonCancel->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonCancel, &ISPushButton::clicked, this, &ISInputDialog::close);
	LayoutBottom->addWidget(ButtonCancel);
}
//-----------------------------------------------------------------------------
ISInputDialog::~ISInputDialog()
{

}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetString(const QString &Title, const QString &LabelText, const QVariant &Value)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_String);
	InputDialog.SetValue(Value);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetText(const QString &Title, const QString &LabelText, const QVariant &Value)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_Text);
	InputDialog.SetValue(Value);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetInteger(const QString &Title, const QString &LabelText, int Minimum, int Maximum, const QVariant &Value)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_Int);
	InputDialog.SetValue(Value);

	ISIntegerEdit *IntegerEdit = dynamic_cast<ISIntegerEdit*>(InputDialog.GetEditWidget());

	if (Minimum)
	{
		IntegerEdit->SetMinimum(Minimum);
	}

	if (Maximum)
	{
		IntegerEdit->SetMaximum(Maximum);
	}

	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toInt();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetDateTime(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_DateTime);

	if (InputDialog.Exec())
	{
		QDateTime DateTime = InputDialog.GetValue().toDateTime();
		DateTime.setTime(QTime(DateTime.time().hour(), DateTime.time().minute()));
		return DateTime;
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetDate(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_Date);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toDate();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetTime(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_Time);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toTime();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetPassword(const QString &Title, const QString &LabelText)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_String, QString("ISPasswordEdit"));
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetColor(const QString &Title, const QString &LabelText, const QVariant &Color)
{
	ISInputDialog InputDialog(Title, LabelText);
	InputDialog.SetEditWidget(ISNamespace::FT_Color);
	InputDialog.SetValue(Color);
	if (InputDialog.Exec())
	{
		return InputDialog.GetValue().toString();
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetList(const QString &Title, const QString &LabelText, const QVariantMap &VariantMap)
{
	ISInputDialog InputDialog(Title, LabelText);

	ISComboEdit *ComboEdit = new ISComboEdit(&InputDialog);
	ComboEdit->setSizePolicy(QSizePolicy::Minimum, ComboEdit->sizePolicy().verticalPolicy());
	ComboEdit->SetEditable(false);

	for (const auto &MapItem : VariantMap.toStdMap())
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
void ISInputDialog::SetEditWidget(ISNamespace::FieldType DataType, const QString &WidgetEditName)
{
	IS_ASSERT(!AddingFieldEdit, "FieldEditBase alredy adding");
	FieldEditBase = ISGui::CreateColumnForField(this, DataType, WidgetEditName);
	LayoutField->addWidget(FieldEditBase, 0, Qt::AlignLeft);
	AddingFieldEdit = true;
}
//-----------------------------------------------------------------------------
void ISInputDialog::SetEditWidget(ISFieldEditBase *EditWidget)
{
	FieldEditBase = EditWidget;
	LayoutField->addWidget(FieldEditBase, 0, Qt::AlignLeft);
	AddingFieldEdit = true;
}
//-----------------------------------------------------------------------------
void ISInputDialog::SetValue(const QVariant &Value)
{
	FieldEditBase->SetValue(Value);
	FieldEditBase->SetFocus();
}
//-----------------------------------------------------------------------------
QVariant ISInputDialog::GetValue() const
{
	return Value;
}
//-----------------------------------------------------------------------------
ISFieldEditBase* ISInputDialog::GetEditWidget()
{
	return FieldEditBase;
}
//-----------------------------------------------------------------------------
void ISInputDialog::Apply()
{
	Value = FieldEditBase->GetValue();
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISInputDialog::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	FieldEditBase->SetFocus();
	adjustSize();
	ForbidResize();
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
