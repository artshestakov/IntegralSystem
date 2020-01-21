#include "StdAfx.h"
#include "ISListEditPopup.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISMetaDataHelper.h"
#include "PMetaClassForeign.h"
#include "ISSystem.h"
#include "ISPushButton.h"
#include "ISMetaData.h"
#include "ISServiceButton.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISListEditPopup::ISListEditPopup(PMetaClassForeign *meta_foreign, QWidget *ComboBox) : ISInterfaceForm(ComboBox)
{
	MetaForeign = meta_foreign;
	MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->GetForeignClass());

	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QVBoxLayout *LayoutFrame = new QVBoxLayout();
	LayoutFrame->setContentsMargins(LAYOUT_MARGINS_5_PX);

	QFrame *Frame = new QFrame(this);
	Frame->setFrameShape(QFrame::Box);
	Frame->setFrameShadow(QFrame::Raised);
	Frame->setLayout(LayoutFrame);
	GetMainLayout()->addWidget(Frame);

	LineEdit = new ISLineEdit(this);
	LineEdit->SetPlaceholderText(LOCALIZATION("Search"));
	LineEdit->SetIcon(BUFFER_ICONS("Search"));
	connect(LineEdit, &ISLineEdit::ValueChange, this, &ISListEditPopup::Search);
	LayoutFrame->addWidget(LineEdit);

	LabelName = new QLabel(this);
	LabelName->setText(MetaTableForeign->GetLocalListName() + ":");
	LabelName->setFont(FONT_APPLICATION_BOLD);
	LabelName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LayoutFrame->addWidget(LabelName);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	connect(ListWidget, &QListWidget::itemClicked, this, &ISListEditPopup::ItemClicked);
	LayoutFrame->addWidget(ListWidget);

	QStatusBar *StatusBar = new QStatusBar(this);
	LayoutFrame->addWidget(StatusBar);

	LabelCountRow = new QLabel(StatusBar);
	StatusBar->addWidget(LabelCountRow);

	LabelSearch = new QLabel(StatusBar);
	LabelSearch->setVisible(false);
	StatusBar->addWidget(LabelSearch);

	if (!MetaTableForeign->GetShowOnly())
	{
		ISServiceButton *ButtonAdd = new ISServiceButton(this);
		ButtonAdd->setFlat(true);
		ButtonAdd->setToolTip(LOCALIZATION("ClickFromAddNewrecordInList"));
		ButtonAdd->setIcon(BUFFER_ICONS("Add"));
		connect(ButtonAdd, &ISServiceButton::clicked, this, &ISListEditPopup::Add);
		StatusBar->addWidget(ButtonAdd);
	}

	ISPushButton *ButtonHide = new ISPushButton(StatusBar);
	ButtonHide->setText(LOCALIZATION("Hide"));
	ButtonHide->setIcon(BUFFER_ICONS("ArrowUp"));
	ButtonHide->setFlat(true);
	connect(ButtonHide, &ISPushButton::clicked, this, &ISListEditPopup::Hide);
	StatusBar->addPermanentWidget(ButtonHide);

	LabelEmpty = new QLabel(this);
	LabelEmpty->setText(LOCALIZATION("ListIsEmpty"));
	LabelEmpty->setFont(FONT_TAHOMA_12_BOLD);
	LabelEmpty->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LabelEmpty->setVisible(false);
}
//-----------------------------------------------------------------------------
ISListEditPopup::~ISListEditPopup()
{

}
//-----------------------------------------------------------------------------
void ISListEditPopup::showEvent(QShowEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	
	LoadDataFromQuery();
	LineEdit->SetFocus();
	ISInterfaceForm::showEvent(e);

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::hideEvent(QHideEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ListWidget->Clear();
	LineEdit->Clear();
	CurrentValue.clear();
	ISInterfaceForm::hideEvent(e);
	ISGui::SetWaitGlobalCursor(false);
	emit Hided();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::paintEvent(QPaintEvent *e)
{
	ISInterfaceForm::paintEvent(e);

	if (LabelEmpty->isVisible())
	{
		QRect Rect = ListWidget->frameGeometry();
		QPoint CenterPoint = Rect.center();
		CenterPoint.setX(CenterPoint.x() - (LabelEmpty->width() / 2));
		CenterPoint.setY(CenterPoint.y() - (LabelEmpty->height() / 2));
		LabelEmpty->move(CenterPoint);
		LabelEmpty->adjustSize();
	}
}
//-----------------------------------------------------------------------------
void ISListEditPopup::SetCurrentValue(const QVariant &current_value)
{
	CurrentValue = current_value;
}
//-----------------------------------------------------------------------------
QString ISListEditPopup::GetSqlFilter() const
{
	return SqlFilter;
}
//-----------------------------------------------------------------------------
void ISListEditPopup::SetSqlFilter(const QString &sql_filter)
{
	SqlFilter = sql_filter;
}
//-----------------------------------------------------------------------------
void ISListEditPopup::ClearSqlFilter()
{
	SqlFilter.clear();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::Search(const QVariant &value)
{
	ISGui::SetWaitGlobalCursor(true);

	QString SearchValue = value.toString().toLower();
	if (SearchValue.length())
	{
		int Founded = 0;
		for (int i = 0; i < ListWidget->count(); i++)
		{
			QListWidgetItem *ListWidgetItem = ListWidget->item(i);
			if (ListWidgetItem->text().toLower().contains(SearchValue))
			{
				Founded++;
				ListWidget->setItemHidden(ListWidgetItem, false);
			}
			else
			{
				ListWidget->setItemHidden(ListWidgetItem, true);
			}
		}

		if (Founded)
		{
			QListWidgetItem *ListWidgetItem = ListWidget->item(0);
			ListWidget->setItemSelected(ListWidgetItem, true);
			ListWidget->setCurrentItem(ListWidgetItem);
		}

		LabelSearch->setVisible(true);
		LabelSearch->setText(LOCALIZATION("Founded") + ": " + QString::number(Founded));
	}
	else
	{
		for (int i = 0; i < ListWidget->count(); i++)
		{
			ListWidget->setItemHidden(ListWidget->item(i), false);
		}

		LabelSearch->setVisible(false);
		LabelSearch->clear();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::ItemClicked(QListWidgetItem *ListWidgetItem)
{
	emit Selected(ListWidgetItem->data(Qt::UserRole), ListWidgetItem->text());
	Hide();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::Add()
{
	Hide();
	ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTableForeign->GetName())->show();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::LoadDataFromQuery()
{
	QString QueryText = MetaForeign->property("SqlQuery").toString();
	if (SqlFilter.length())
	{
		QueryText = ISMetaDataHelper::GenerateSqlQueryFromForeign(dynamic_cast<PMetaClassForeign*>(MetaForeign), SqlFilter);
	}

	ISQuery qSelect(QueryText);
	if (qSelect.Execute())
	{
		QListWidgetItem *CurrentItem = nullptr;

		while (qSelect.Next())
		{
			QVariant ID = qSelect.ReadColumn("ID");
			QString Value = qSelect.ReadColumn("Value").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(Value);
			ListWidgetItem->setData(Qt::UserRole, ID);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 25));
			
			if (CurrentValue == ID)
			{
				CurrentItem = ListWidgetItem;
			}
		}

		if (CurrentItem)
		{
			ListWidget->setItemSelected(CurrentItem, true);
			ListWidget->scrollToItem(CurrentItem, QAbstractItemView::ScrollHint::PositionAtCenter);
			CurrentItem->setFont(FONT_APPLICATION_BOLD);
			CurrentItem->setSelected(true);
		}

		LabelCountRow->setText(LOCALIZATION("RecordsCount") + ": " + QString::number(ListWidget->count()));
	}

	LabelEmpty->setVisible(!ListWidget->count());
}
//-----------------------------------------------------------------------------
void ISListEditPopup::Hide()
{
	HideAnimation(300);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::EnterClicked()
{
	QListWidgetItem *ListWidgetItem = ListWidget->currentItem();
	if (ListWidgetItem)
	{
		ItemClicked(ListWidgetItem);
	}
}
//-----------------------------------------------------------------------------
void ISListEditPopup::EscapeClicked()
{
	Hide();
}
//-----------------------------------------------------------------------------
