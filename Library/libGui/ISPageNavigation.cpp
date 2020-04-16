#include "ISPageNavigation.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISInputDialog.h"
//-----------------------------------------------------------------------------
ISPageNavigation::ISPageNavigation(QWidget *parent) : QWidget(parent)
{
	RowCount = -1;
	PageCount = 0;
	CurrentPage = 0;
	Limit = 0;
	Offset = 0;

	QHBoxLayout *Layout = new QHBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(Layout);

	ButtonBegin = new ISServiceButton(this);
	ButtonBegin->setEnabled(false);
	ButtonBegin->setToolTip(LANG("Page.Begin"));
	ButtonBegin->setIcon(BUFFER_ICONS("PageNavigation.Begin"));
	ButtonBegin->setFlat(true);
	ButtonBegin->setFixedSize(ISDefines::Gui::SIZE_22_22);
	ButtonBegin->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonBegin, &QToolButton::clicked, this, &ISPageNavigation::BeginClicked);
	Layout->addWidget(ButtonBegin);

	ButtonPrevious = new ISServiceButton(this);
	ButtonPrevious->setEnabled(false);
	ButtonPrevious->setToolTip(LANG("Page.Previous"));
	ButtonPrevious->setIcon(BUFFER_ICONS("PageNavigation.Previous"));
	ButtonPrevious->setFlat(true);
	ButtonPrevious->setFixedSize(ISDefines::Gui::SIZE_22_22);
	ButtonPrevious->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonPrevious, &QToolButton::clicked, this, &ISPageNavigation::PreviousClicked);
	Layout->addWidget(ButtonPrevious);

	ButtonSelect = new ISPushButton(this);
	ButtonSelect->setText(LANG("Page.Select").arg(0).arg(0));
	ButtonSelect->setToolTip(LANG("ClickFromSelectNumberPage"));
	ButtonSelect->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonSelect, &QToolButton::clicked, this, &ISPageNavigation::Select);
	Layout->addWidget(ButtonSelect);

	ButtonNext = new ISServiceButton(this);
	ButtonNext->setToolTip(LANG("Page.Next"));
	ButtonNext->setIcon(BUFFER_ICONS("PageNavigation.Next"));
	ButtonNext->setFlat(true);
	ButtonNext->setFixedSize(ISDefines::Gui::SIZE_22_22);
	ButtonNext->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonNext, &QToolButton::clicked, this, &ISPageNavigation::NextClicked);
	Layout->addWidget(ButtonNext);

	ButtonEnd = new ISServiceButton(this);
	ButtonEnd->setToolTip(LANG("Page.End"));
	ButtonEnd->setIcon(BUFFER_ICONS("PageNavigation.End"));
	ButtonEnd->setFlat(true);
	ButtonEnd->setFixedSize(ISDefines::Gui::SIZE_22_22);
	ButtonEnd->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonEnd, &QToolButton::clicked, this, &ISPageNavigation::EndClicked);
	Layout->addWidget(ButtonEnd);
}
//-----------------------------------------------------------------------------
ISPageNavigation::~ISPageNavigation()
{

}
//-----------------------------------------------------------------------------
void ISPageNavigation::SetRowCount(int row_count)
{
	if (row_count)
	{
		RowCount = row_count;

		PageCount = RowCount / Limit;
		if (row_count % Limit)
		{
			++PageCount;
		}

		ButtonSelect->setText(LANG("Page.Select").arg(CurrentPage + 1).arg(PageCount));

		if (PageCount == 1) //Если количество страниц одна
		{
			ButtonBegin->setEnabled(false);
			ButtonPrevious->setEnabled(false);
			ButtonSelect->setEnabled(false);
			ButtonNext->setEnabled(false);
			ButtonEnd->setEnabled(false);
		}
	}
}
//-----------------------------------------------------------------------------
void ISPageNavigation::SetLimit(int limit)
{
	Limit = limit;
}
//-----------------------------------------------------------------------------
void ISPageNavigation::BeginClicked()
{
	CurrentPage = 0;
	Offset = 0;
	emit OffsetSignal(Offset);
	
	PageChanged();
}
//-----------------------------------------------------------------------------
void ISPageNavigation::PreviousClicked()
{
	--CurrentPage;
	int NewOffset = Offset - Limit;
	Offset = NewOffset;
	emit OffsetSignal(NewOffset);

	PageChanged();
}
//-----------------------------------------------------------------------------
void ISPageNavigation::Select()
{
	int Page = ISInputDialog::GetInteger(LANG("Page"), LANG("InputNumberPage"), 1, PageCount, CurrentPage + 1).toInt();
	if (Page)
	{
		CurrentPage = Page - 1;
		Offset = CurrentPage * Limit;
		emit OffsetSignal(Offset);
		PageChanged();
	}
}
//-----------------------------------------------------------------------------
void ISPageNavigation::NextClicked()
{
	++CurrentPage;

	int NewOffset = Offset + Limit;
	Offset = NewOffset;
	emit OffsetSignal(NewOffset);

	PageChanged();
}
//-----------------------------------------------------------------------------
void ISPageNavigation::EndClicked()
{
	CurrentPage = PageCount;
	--CurrentPage;
	Offset = CurrentPage * Limit;
	emit OffsetSignal(Offset);

	PageChanged();
}
//-----------------------------------------------------------------------------
void ISPageNavigation::PageChanged()
{
	if (CurrentPage + 1 > 1 && CurrentPage + 1 < PageCount) //Если текущая страница не первая и не последняя
	{
		ButtonBegin->setEnabled(true);
		ButtonPrevious->setEnabled(true);
		ButtonNext->setEnabled(true);
		ButtonEnd->setEnabled(true);
	}
	else if (CurrentPage + 1 == 1) //Если текущая страница первая
	{
		ButtonBegin->setEnabled(false);
		ButtonPrevious->setEnabled(false);
		ButtonNext->setEnabled(true);
		ButtonEnd->setEnabled(true);
	}
	else if (CurrentPage + 1 == PageCount) //Если текущая страница последняя
	{
		ButtonBegin->setEnabled(true);
		ButtonPrevious->setEnabled(true);
		ButtonNext->setEnabled(false);
		ButtonEnd->setEnabled(false);
	}

	emit Update();
}
//-----------------------------------------------------------------------------
