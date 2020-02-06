#include "ISComboEdit.h"
#include "ISDefinesGui.h"
#include "ISQLineEdit.h"
#include "ISPopupDelegate.h"
#include "ISConstants.h"
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
	ComboBox->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	ComboBox->setIconSize(DEFINES_GUI.SIZE_22_22);
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
	if (UserData.isNull())
	{
		return QVariant();
	}

	return UserData;
}
//-----------------------------------------------------------------------------
void ISComboEdit::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISComboEdit::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
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
