#include "ISImageEdit.h"
#include "EXDefines.h"
#include "ISGui.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISImageEdit::ISImageEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	CreateButtonClear();

	ImageWidget = new ISImageWidget(this);
	ImageWidget->setFixedSize(QSize(150, 150));
	ImageWidget->setCursor(CURSOR_POINTING_HAND);
	ImageWidget->setToolTip(LANG("ClickRightButtonMouseForCallContextMenu"));
	connect(ImageWidget, &ISImageWidget::ImageChanged, this, &ISImageEdit::ValueChanged);
	AddWidgetEdit(ImageWidget, this);

	AddMainLayoutStretch();
}
//-----------------------------------------------------------------------------
ISImageEdit::~ISImageEdit()
{

}
//-----------------------------------------------------------------------------
void ISImageEdit::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		QPixmap Pixmap = ISGui::ByteArrayToPixmap(value.toByteArray());
		ImageWidget->SetPixmap(Pixmap);
	}
	else
	{
		ImageWidget->Clear();
	}
}
//-----------------------------------------------------------------------------
QVariant ISImageEdit::GetValue() const
{
	QByteArray ByteArray = ISGui::PixmapToByteArray(ImageWidget->GetPixmap());
	if (!ByteArray.isNull())
	{
		return ByteArray;
	}
	
	return QVariant();
}
//-----------------------------------------------------------------------------
void ISImageEdit::Clear()
{
	ImageWidget->Clear();
}
//-----------------------------------------------------------------------------
void ISImageEdit::SetVisibleClear(bool Visible)
{

}
//-----------------------------------------------------------------------------
void ISImageEdit::SetReadOnly(bool read_only)
{
	ImageWidget->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
