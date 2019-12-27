#include "StdAfx.h"
#include "ISRangeEditBase.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISRangeEditBase::ISRangeEditBase(QWidget *parent, const QString &FieldName) : QWidget(parent)
{
	BeginFieldEdit = nullptr;
	EndFieldEdit = nullptr;

	QHBoxLayout *MainLayout = new QHBoxLayout();
	MainLayout->setContentsMargins(LAYOUT_MARGINS_NULL);
	MainLayout->setSpacing(EDIT_FIELD_SPACING);
	setLayout(MainLayout);

	QLabel *LabelFieldName = new QLabel(this);
	LabelFieldName->setFont(FONT_APPLICATION_BOLD);
	MainLayout->addWidget(LabelFieldName);

	if (FieldName.length())
	{
		LabelFieldName->setText(FieldName);
		
	}
	else
	{
		LabelFieldName->setText(LOCALIZATION("Range") + ":");
	}

	LayoutLeft = new QHBoxLayout();
	MainLayout->addLayout(LayoutLeft);

	LayoutBegin = new QHBoxLayout();
	MainLayout->addLayout(LayoutBegin);

	LabelBegin = new QLabel(this);
	LayoutBegin->addWidget(LabelBegin);

	LayoutEnd = new QHBoxLayout();
	MainLayout->addLayout(LayoutEnd);

	LabelEnd = new QLabel(this);
	LayoutEnd->addWidget(LabelEnd);

	LayoutRight = new QHBoxLayout();
	MainLayout->addLayout(LayoutRight);

	MainLayout->addStretch();
}
//-----------------------------------------------------------------------------
ISRangeEditBase::~ISRangeEditBase()
{

}
//-----------------------------------------------------------------------------
void ISRangeEditBase::SetValue(const ISRangeStruct &range)
{
	Q_UNUSED(range);
	std::runtime_error(Q_FUNC_INFO);
}
//-----------------------------------------------------------------------------
ISRangeStruct ISRangeEditBase::GetValue() const
{
	std::runtime_error(Q_FUNC_INFO);
	return ISRangeStruct();
}
//-----------------------------------------------------------------------------
void ISRangeEditBase::AddFields(const QString &BeginString, ISFieldEditBase *begin_field_edit, const QString &EndString, ISFieldEditBase *end_field_edit)
{
	IS_ASSERT(!BeginFieldEdit, "BeginFieldEdit already exist");
	IS_ASSERT(!EndFieldEdit, "EndFieldEdit already exist");

	BeginFieldEdit = begin_field_edit;
	EndFieldEdit = end_field_edit;

	LayoutBegin->addWidget(BeginFieldEdit);
	LayoutEnd->addWidget(EndFieldEdit);

	LabelBegin->setText(BeginString);
	LabelEnd->setText(EndString);
}
//-----------------------------------------------------------------------------
void ISRangeEditBase::AddWidgetToLeft(QWidget *Widget)
{
	LayoutLeft->addWidget(Widget);
}
//-----------------------------------------------------------------------------
void ISRangeEditBase::AddWidgetToRight(QWidget *Widget)
{
	LayoutRight->addWidget(Widget);
}
//-----------------------------------------------------------------------------
