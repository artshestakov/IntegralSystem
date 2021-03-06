#include "ISFieldEditBase.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDialogsCommon.h"
#include "ISAlgorithm.h"
#include "ISConstantsGui.h"
//-----------------------------------------------------------------------------
ISFieldEditBase::ISFieldEditBase(QWidget *parent)
    : QWidget(parent),
    ButtonHint(nullptr),
    EditWidget(nullptr),
    FieldEditPointer(nullptr),
    ModificationFlag(false),
    IsBlinkBorder(false),
    ButtonClear(nullptr),
    ReadOnly(false)
{
    SetSizePolicyHorizontal(QSizePolicy::Minimum);
    SetSizePolicyVertical(QSizePolicy::Maximum);

    //������� �����������
    MainLayout = new QHBoxLayout();
    MainLayout->setContentsMargins(QMargins());
    MainLayout->setSpacing(EDIT_FIELD_SPACING);
    setLayout(MainLayout);

    //����� �����������
    LayoutLeft = new QHBoxLayout();
    MainLayout->addLayout(LayoutLeft);

    //����������� ���������
    LayoutEdit = new QHBoxLayout();
    LayoutEdit->setContentsMargins(QMargins(2, 2, 2, 2));
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
void ISFieldEditBase::SetReadOnly(bool read_only)
{
    ReadOnly = read_only;
    SetVisibleClear(!ReadOnly);
    EditWidget->setFocusPolicy(ReadOnly ? Qt::NoFocus : Qt::StrongFocus);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetEnabled(bool Enabled)
{
    Q_UNUSED(Enabled);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetPlaceholderText(const QString &placeholder_text)
{
    Q_UNUSED(placeholder_text);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetFrameShape(QFrame::Shape FrameShape)
{
    Q_UNUSED(FrameShape);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetRegExp(const QString &RegExp)
{
    Q_UNUSED(RegExp);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetColorText(const QColor &Color)
{
    Q_UNUSED(Color);
}
//-----------------------------------------------------------------------------
void ISFieldEditBase::SetSizePolicy(QSizePolicy::Policy PolicyHorizontal, QSizePolicy::Policy PolicyVertical)
{
    setSizePolicy(PolicyHorizontal, PolicyVertical);
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
bool ISFieldEditBase::IsReadOnly() const
{
    return ReadOnly;
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
void ISFieldEditBase::BlinkRed()
{
    Blink(QColor(236, 99, 99));
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
        ISSleep(100);
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
            ButtonHint->setFixedSize(QSize(22, 22));
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
    if (EditWidget)
    {
        EditWidget->setFocus();
    }
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
        ButtonClear = new ISServiceButton(BUFFER_ICONS("ButtonClear"), LANG("Clear.Field"), this);
        ButtonClear->setFocusPolicy(Qt::NoFocus);
        ButtonClear->setFlat(true);
        connect(ButtonClear, &ISServiceButton::clicked, this, &ISFieldEditBase::Clear);
        connect(ButtonClear, &ISServiceButton::clicked, this, &ISFieldEditBase::SetFocus);
        MainLayout->addWidget(ButtonClear);
    }
}
//-----------------------------------------------------------------------------
