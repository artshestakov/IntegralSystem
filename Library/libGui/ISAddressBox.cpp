#include "ISAddressBox.h"
#include "ISQLineEdit.h"
#include "ISDelegates.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISAddressBox::ISAddressBox(QWidget *parent) : QComboBox(parent)
{
	setEditable(true);
	setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	setItemDelegate(new ISPopupDelegate(this));
	completer()->setMaxVisibleItems(20);
	completer()->setFilterMode(Qt::MatchContains);
	completer()->setCaseSensitivity(Qt::CaseInsensitive);
	completer()->setCompletionMode(QCompleter::PopupCompletion);
	connect(this, static_cast<void(ISAddressBox::*)(int)>(&ISAddressBox::activated), this, &ISAddressBox::Activated);

	QAction *ActionClear = new QAction(this);
	ActionClear->setToolTip(LANG("Clear.Field"));
	ActionClear->setIcon(BUFFER_ICONS("ButtonClear"));
	connect(ActionClear, &QAction::triggered, this, &ISAddressBox::ClearClicked);
	lineEdit()->addAction(ActionClear, QLineEdit::TrailingPosition);
}
//-----------------------------------------------------------------------------
ISAddressBox::~ISAddressBox()
{

}
//-----------------------------------------------------------------------------
void ISAddressBox::Clear()
{
	clear();
	ClearClicked();
}
//-----------------------------------------------------------------------------
void ISAddressBox::ClearClicked()
{
	clearEditText();
	CurrentValue.clear();
	emit ChangeValue(CurrentValue);
}
//-----------------------------------------------------------------------------
void ISAddressBox::Activated(int Index)
{
	QVariant ItemData = itemData(Index);
	if (ItemData != CurrentValue)
	{
		CurrentValue = ItemData;
		emit ChangeValue(CurrentValue);
	}
}
//-----------------------------------------------------------------------------
