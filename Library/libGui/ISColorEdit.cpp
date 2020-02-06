#include "ISColorEdit.h"
#include "ISDefinesGui.h"
#include "ISServiceButton.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISColorEdit::ISColorEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	CreateButtonClear();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setLayout(new QHBoxLayout());
	GroupBox->layout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_NULL);
	GroupBox->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	AddWidgetEdit(GroupBox, this);

	WidgetColor = new QWidget(this);
	WidgetColor->setAutoFillBackground(true);
	GroupBox->layout()->addWidget(WidgetColor);

	ISServiceButton *ButtonSelectColor = new ISServiceButton(this);
	ButtonSelectColor->setToolTip(LANG("SelectColor") + "...");
	ButtonSelectColor->setIcon(BUFFER_ICONS("Color"));
	ButtonSelectColor->setFocusPolicy(Qt::NoFocus);
	connect(ButtonSelectColor, &ISServiceButton::clicked, this, &ISColorEdit::SelectColor);
	AddWidgetToRight(ButtonSelectColor);

	SetFixedWidth(200);
}
//-----------------------------------------------------------------------------
ISColorEdit::~ISColorEdit()
{

}
//-----------------------------------------------------------------------------
void ISColorEdit::SetValue(const QVariant &value)
{
	if (value.toString().length())
	{
		QColor Color(value.toString());
		QPalette Palette = WidgetColor->palette();
		Palette.setColor(QPalette::Background, Color);
		WidgetColor->setPalette(Palette);
		WidgetColor->setToolTip(QString("RGB(%1, %2, %3)").arg(Color.red()).arg(Color.green()).arg(Color.blue()));
	}
}
//-----------------------------------------------------------------------------
QVariant ISColorEdit::GetValue() const
{
	QColor Color = WidgetColor->palette().color(QPalette::Background);
	return Color.name();
}
//-----------------------------------------------------------------------------
void ISColorEdit::Clear()
{
	WidgetColor->setPalette(QPalette());
	WidgetColor->setToolTip(QString());
}
//-----------------------------------------------------------------------------
void ISColorEdit::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISColorEdit::SetReadOnly(bool read_only)
{
	WidgetColor->setEnabled(!read_only);
}
//-----------------------------------------------------------------------------
void ISColorEdit::SelectColor()
{
	QColor Color = QColorDialog::getColor(WidgetColor->palette().color(QPalette::Background), this, LANG("SelectedColor"), QColorDialog::DontUseNativeDialog);
	QString ColorName = Color.name();
	if (ColorName != COLOR_STANDART)
	{
		SetValue(ColorName);
		emit ValueChanged();
	}
}
//-----------------------------------------------------------------------------
