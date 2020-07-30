#include "ISImageEdit.h"
#include "ISConstants.h"
#include "ISGui.h"
#include "ISLocalization.h"
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
	value.isValid() ? ImageWidget->SetByteArray(value.toByteArray()) : ImageWidget->Clear();
}
//-----------------------------------------------------------------------------
QVariant ISImageEdit::GetValue() const
{
	QByteArray ByteArray = ImageWidget->GetImage();
	return ByteArray.isEmpty() ? QVariant() : ByteArray;
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
