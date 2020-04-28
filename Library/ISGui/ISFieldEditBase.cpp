#include "ISFieldEditBase.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISFieldEditBase::ISFieldEditBase(QWidget *parent)
	: QWidget(parent),
	ButtonHint(nullptr),
	EditWidget(nullptr),
	FieldEditPointer(nullptr),
	ModificationFlag(false),
	IsBlinkBorder(false),
	ButtonClear(nullptr)
{
	SetSizePolicyHorizontal(QSizePolicy::Minimum);
	SetSizePolicyVertical(QSizePolicy::Maximum);

	//������� �����������
	MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	MainLayout->setSpacing(EDIT_FIELD_SPACING);
	setLayout(MainLayout);

	//����� �����������
	LayoutLeft = new QHBoxLayout();
	MainLayout->addLayout(LayoutLeft);

	//����������� ���������
	LayoutEdit = new QHBoxLayout();
	LayoutEdit->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_2_PX);
	MainLayout->addLayout(LayoutEdit);

	//������ �����������
	LayoutRight = new QHBoxLayout();
	MainLayout->addLayout(LayoutRight);
}
//-----------------------------------------------------------------------------
ISFieldEditBase::~ISFieldEditBase()
{

}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetValue(const QVariant &value)
{
    Q_UNUSED(value);
	std::runtime_error(Q_FUNC_INFO);
}
//-----------------------------------------------------------------------------
QVariant ISFieldEditBase::GetValue() const
{
	std::runtime_error(Q_FUNC_INFO);
	return QVariant();
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::Clear()
{
	std::runtime_error(Q_FUNC_INFO);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::Invoke()
{

}
//-----------------------------------------------------------------------------
bool ISFieldEditBase::IsValid() const
{
	return true;
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetFont(const QFont &Font)
{
	EditWidget->setFont(Font);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetReadOnly(bool ReadOnly)
{
	SetVisibleClear(!ReadOnly);
	if (ReadOnly)
	{
		EditWidget->setFocusPolicy(Qt::NoFocus);
	}
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetPlaceholderText(const QString &placeholder_text)
{

}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetRegExp(const QString &RegExp)
{

}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetCursor(const QCursor &Cursor)
{
	EditWidget->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetToolTip(const QString &ToolTip)
{
	EditWidget->setToolTip(ToolTip);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetFixedWidth(int Width)
{
	EditWidget->setFixedWidth(Width);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetSizePolicyHorizontal(QSizePolicy::Policy PolicyHorizontal)
{
	setSizePolicy(QSizePolicy(PolicyHorizontal, sizePolicy().verticalPolicy()));
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetSizePolicyVertical(QSizePolicy::Policy PolicyVertical)
{
	setSizePolicy(QSizePolicy(sizePolicy().horizontalPolicy(), PolicyVertical));
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetVisibleClear(bool Visible)
{
	if (ButtonClear)
	{
		ButtonClear->setVisible(Visible);
	}
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::BlinkRed()
{
	Blink(ISDefines::Gui::EDIT_WIDGET_COLOR_RED);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::Blink(const QColor &Color)
{
	BlinkColor = Color;
	EditWidget->clearFocus();
	for (size_t i = 0; i < EDIT_BLINK_COUNT; i++)
	{
		IsBlinkBorder = IsBlinkBorder ? false : true;
		repaint();
		ISSystem::SleepMilliseconds(100);
	}
	SetFocus();
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetModificationFlag(bool modification_flag)
{
	ModificationFlag = modification_flag;
}
//-----------------------------------------------------------------------------
bool ISFieldEditBase::GetModificationFlag() const
{
	return ModificationFlag;
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::CreateHint(const QString &Hint)
{
	if (!Hint.isEmpty())
	{
		if (!ButtonHint)
		{
			ButtonHint = new QToolButton(this);
			ButtonHint->setFocusPolicy(Qt::NoFocus);
			ButtonHint->setIcon(BUFFER_ICONS("Hint"));
			ButtonHint->setAutoRaise(true);
			ButtonHint->setFixedSize(ISDefines::Gui::SIZE_22_22);
			ButtonHint->setCursor(CURSOR_POINTING_HAND);
			ButtonHint->setToolTip(LANG("ClickToViewHelp"));
			connect(ButtonHint, &QToolButton::clicked, this, &ISFieldEditBase::ShowHint);
			LayoutLeft->addWidget(ButtonHint);
		}
		ButtonHint->setAccessibleDescription(Hint);
	}
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetFocus()
{
	IS_ASSERT(EditWidget, "Null EditWidget");
	EditWidget->setFocus();
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::paintEvent(QPaintEvent *PaintEvent)
{
	QWidget::paintEvent(PaintEvent);
	if (EditWidget)
	{
		if (IsBlinkBorder)
		{
			QPainter Painter(this);
			Painter.setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
			Painter.setPen(QPen(BlinkColor, 2.5, Qt::SolidLine));
			Painter.drawRect(EditWidget->pos().x() - 1, EditWidget->pos().y() - 1, EditWidget->width() + 2, EditWidget->height() + 2);
		}
	}
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::ShowHint()
{
	QToolButton *SenderButton = dynamic_cast<QToolButton*>(sender());
	if (SenderButton)
	{
		ISMessageBox::ShowInformation(this, SenderButton->accessibleDescription());
	}
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::ValueChanged()
{
	SetModificationFlag(true);
	emit DataChanged();
	emit ValueChange(GetValue());
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::AddWidgetEdit(QWidget *edit_widget, QWidget *field_edit_pointer)
{
	IS_ASSERT(!EditWidget, "EditWidget already exist");
	EditWidget = edit_widget;
	FieldEditPointer = field_edit_pointer;
	LayoutEdit->addWidget(edit_widget);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::AddWidgetEditStretch()
{
	LayoutEdit->addStretch();
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::AddMainLayoutStretch()
{
	MainLayout->addStretch();
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::AddWidgetToLeft(QWidget *widget)
{
	LayoutLeft->addWidget(widget);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::AddWidgetToRight(QWidget *widget)
{
	LayoutRight->addWidget(widget);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::CreateButtonClear()
{
	if (!ButtonClear)
	{
		ButtonClear = new ISButtonClear(this);
		connect(ButtonClear, &ISButtonClear::clicked, this, &ISFieldEditBase::Clear);
		connect(ButtonClear, &ISButtonClear::clicked, this, &ISFieldEditBase::SetFocus);
		MainLayout->addWidget(ButtonClear);
	}
}
//-----------------------------------------------------------------------------