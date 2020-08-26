#include "ISListEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISMetaDataHelper.h"
#include "ISUserRoleEntity.h"
#include "ISGui.h"
#include "ISMessageBox.h"
#include "ISControls.h"
#include "ISDefinesGui.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
ISListEditPopup::ISListEditPopup(PMetaForeign *meta_foreign, QWidget *ComboBox)
	: ISInterfaceForm(ComboBox),
	MetaForeign(meta_foreign),
	MetaTableForeign(ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass))
{
	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QVBoxLayout *LayoutFrame = new QVBoxLayout();
	LayoutFrame->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);

	QFrame *Frame = new QFrame(this);
	Frame->setFrameShape(QFrame::Box);
	Frame->setFrameShadow(QFrame::Raised);
	Frame->setLayout(LayoutFrame);
	GetMainLayout()->addWidget(Frame);

	LineEdit = new ISLineEdit(this);
	LineEdit->SetPlaceholderText(LANG("Search"));
	LineEdit->SetIcon(BUFFER_ICONS("Search"));
	connect(LineEdit, &ISLineEdit::ValueChange, this, &ISListEditPopup::Search);
	LayoutFrame->addWidget(LineEdit);

	LabelName = new QLabel(MetaTableForeign->LocalListName + ':', this);
	LabelName->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
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

	if (!MetaTableForeign->ShowOnly)
	{
		ISServiceButton *ButtonAdd = new ISServiceButton(BUFFER_ICONS("Add"), LANG("ClickFromAddNewrecordInList"), this);
		ButtonAdd->setFlat(true);
		connect(ButtonAdd, &ISServiceButton::clicked, this, &ISListEditPopup::Add);
		StatusBar->addWidget(ButtonAdd);
	}

	ISPushButton *ButtonHide = new ISPushButton(BUFFER_ICONS("ArrowUp"), LANG("Hide"), StatusBar);
	ButtonHide->setFlat(true);
	connect(ButtonHide, &ISPushButton::clicked, this, &ISListEditPopup::hide);
	StatusBar->addPermanentWidget(ButtonHide);

	LabelEmpty = new QLabel(LANG("ListIsEmpty"), this);
	LabelEmpty->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
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
		QPoint CenterPoint = ListWidget->frameGeometry().center();
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
	if (SearchValue.isEmpty())
	{
		ListWidget->SetVisibleItems(true);
		LabelSearch->setVisible(false);
		LabelSearch->clear();
	}
	else
	{
		int Founded = 0;
		for (int i = 0; i < ListWidget->count(); ++i)
		{
			QListWidgetItem *ListWidgetItem = ListWidget->item(i);
			if (ListWidgetItem->text().toLower().contains(SearchValue))
			{
				++Founded;
				ListWidget->setItemHidden(ListWidgetItem, false);
			}
			else
			{
				ListWidget->setItemHidden(ListWidgetItem, true);
			}
		}

		if (Founded)
		{
			QListWidgetItem *ListWidgetItem = ListWidget->BeginItem();
			ListWidget->setItemSelected(ListWidgetItem, true);
			ListWidget->setCurrentItem(ListWidgetItem);
		}
		LabelSearch->setVisible(true);
		LabelSearch->setText(LANG("Founded") + ": " + QString::number(Founded));
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISListEditPopup::ItemClicked(QListWidgetItem *ListWidgetItem)
{
	emit Selected(ListWidgetItem->data(Qt::UserRole), ListWidgetItem->text());
	hide();
}
//-----------------------------------------------------------------------------
void ISListEditPopup::Add()
{
	hide();
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTableForeign->Name));
}
//-----------------------------------------------------------------------------
void ISListEditPopup::LoadDataFromQuery()
{
	QString QueryText = MetaForeign->SqlQuery;
	if (!SqlFilter.isEmpty())
	{
		QueryText = ISMetaDataHelper::GenerateSqlQueryFromForeign(MetaForeign, SqlFilter);
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
			CurrentItem = CurrentValue == ID ? ListWidgetItem : nullptr;
		}

		if (CurrentItem)
		{
			ListWidget->setItemSelected(CurrentItem, true);
			ListWidget->scrollToItem(CurrentItem, QAbstractItemView::ScrollHint::PositionAtCenter);
			CurrentItem->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
			CurrentItem->setSelected(true);
		}
		LabelCountRow->setText(LANG("RecordsCount") + ": " + QString::number(ListWidget->count()));
	}
	LabelEmpty->setVisible(!ListWidget->count());
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
	hide();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISListEdit::ISListEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	CreateButtonClear();

	ButtonMain = new ISPushButton(this);
	ButtonMain->setText(LANG("NotSelected"));
	ButtonMain->setToolTip(LANG("ClickFromShowList"));
	ButtonMain->setIcon(BUFFER_ICONS("ArrowDown"));
	ButtonMain->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	ButtonMain->setCursor(CURSOR_POINTING_HAND);
	ButtonMain->setStyleSheet(ButtonMain->styleSheet() + STYLE_SHEET("QPushButtonListEdit"));
	ButtonMain->setCheckable(true);
	connect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup);
	AddWidgetEdit(ButtonMain, this);

	ButtonList = new QToolButton(this);
	ButtonList->setMinimumHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	ButtonList->setIcon(BUFFER_ICONS("Search"));
	ButtonList->setToolTip(LANG("OpenList"));
	ButtonList->setCursor(CURSOR_POINTING_HAND);
	ButtonList->setAutoRaise(true);
	ButtonList->setFocusPolicy(Qt::NoFocus);
	connect(ButtonList, &QToolButton::clicked, this, &ISListEdit::ShowListForm);
	AddWidgetToRight(ButtonList);
}
//-----------------------------------------------------------------------------
ISListEdit::~ISListEdit()
{

}
//-----------------------------------------------------------------------------
void ISListEdit::SetValue(const QVariant &value)
{
	ID = value;

	if (!value.isValid())
	{
		Clear();
		return;
	}

	ISQuery qSelect(ISMetaDataHelper::GenerateSqlQueryFromForeign(MetaForeign, QString(), ID));
	qSelect.BindValue(":ObjectID", ID);
	if (qSelect.ExecuteFirst())
	{
		QString Name = qSelect.ReadColumn(1).toString();
		ButtonMain->setText(Name);
		ValueChanged();
	}

	if (ActionEdit)
	{
		ActionEdit->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
QVariant ISListEdit::GetValue() const
{
	return ID;
}
//-----------------------------------------------------------------------------
void ISListEdit::Clear()
{
	ID.clear();
	ButtonMain->setText(LANG("NotSelected"));
	ValueChanged();

	if (ActionEdit)
	{
		ActionEdit->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::SetReadOnly(bool read_only)
{
	ISFieldEditBase::SetReadOnly(read_only);
	ButtonMain->setCheckable(!read_only);
	ButtonList->setVisible(!read_only);

	if (read_only)
	{
		ButtonMain->setIcon(QIcon());
		disconnect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup);
	}
	else
	{
		ButtonMain->setIcon(BUFFER_ICONS("ArrowDown"));
		connect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup);
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::InvokeList(PMetaForeign *meta_foreign)
{
	MetaForeign = meta_foreign;
	MetaTable = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);

	ListEditPopup = new ISListEditPopup(MetaForeign, this);
	connect(ListEditPopup, &ISListEditPopup::Selected, this, &ISListEdit::SelectedValue);
	connect(ListEditPopup, &ISListEditPopup::Hided, this, &ISListEdit::HidedPopup);

	if (MetaTable->ShowOnly)
	{
		ButtonList->setPopupMode(QToolButton::DelayedPopup);

		ActionCreate = nullptr;
		ActionEdit = nullptr;
	}
	else
	{
		QMenu *Menu = new QMenu(ButtonList);
		ButtonList->setMenu(Menu);
		ButtonList->setPopupMode(QToolButton::MenuButtonPopup);

		ActionCreate = ISControls::CreateActionCreate(ButtonList->menu());
		connect(ActionCreate, &QAction::triggered, this, &ISListEdit::CreateObject);
		Menu->addAction(ActionCreate);

		ActionEdit = ISControls::CreateActionEdit(ButtonList->menu());
		ActionEdit->setEnabled(false);
		connect(ActionEdit, &QAction::triggered, this, &ISListEdit::EditObject);
		Menu->addAction(ActionEdit);
	}
}
//-----------------------------------------------------------------------------
QString ISListEdit::GetCurrentText() const
{
	return ButtonMain->text();
}
//-----------------------------------------------------------------------------
void ISListEdit::SetEnabled(bool Enabled)
{
	ButtonMain->setEnabled(Enabled);
	ButtonList->setEnabled(Enabled);
}
//-----------------------------------------------------------------------------
void ISListEdit::SetSqlFilter(const QString &sql_filter)
{
	ListEditPopup->SetSqlFilter(sql_filter);
}
//-----------------------------------------------------------------------------
void ISListEdit::ClearSqlFilter()
{
	ListEditPopup->ClearSqlFilter();
}
//-----------------------------------------------------------------------------
void ISListEdit::SelectedValue(const QVariant &id, const QString &text)
{
	ID = id;
	ButtonMain->setText(text);
	ValueChanged();

	if (ActionEdit)
	{
		ActionEdit->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::ShowPopup()
{
	ButtonMain->setIcon(BUFFER_ICONS("ArrowUp"));
	ButtonMain->setChecked(true);

	ListEditPopup->SetCurrentValue(ID);
	ListEditPopup->adjustSize();
	ListEditPopup->resize(ButtonMain->width(), ListEditPopup->height());

	QPoint PointButton = ButtonMain->mapToGlobal(QPoint());
	QRect ScreenGeometry = QDesktopWidget().screenGeometry();

	QPoint PointNew = PointButton;
	PointNew.setY(PointNew.y() + ButtonMain->height() - 1);

	if (PointNew.y() + ListEditPopup->height() > ScreenGeometry.height()) //Если виджет выходит за рамку экрана по высоте (внизу)
	{
		PointNew = PointButton;
		PointNew.setY(PointNew.y() - ListEditPopup->height() - 2);
	}

	if (PointNew.x() + ListEditPopup->width() > ScreenGeometry.width()) //Если виджет выходит за рамку экрана по ширине (справа)
	{
		ListEditPopup->resize(ScreenGeometry.width() - PointButton.x(), ListEditPopup->height());
	}

	ListEditPopup->move(PointNew);
	ListEditPopup->show();
}
//-----------------------------------------------------------------------------
void ISListEdit::HidedPopup()
{
	ButtonMain->setIcon(BUFFER_ICONS("ArrowDown"));
	ButtonMain->setChecked(false);
}
//-----------------------------------------------------------------------------
void ISListEdit::ShowListForm()
{
	int SelectedID = ISGui::SelectObject(MetaTable->Name, GetValue().toInt());
	if (SelectedID)
	{
		SetValue(SelectedID);
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::CreateObject()
{
	if (ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->Name));
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->LocalListName));
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::EditObject()
{
	if (ISUserRoleEntity::Instance().CheckAccessTable(MetaTable->UID, CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->Name, GetValue().toInt()));
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->LocalListName));
	}

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExecutorEdit::ISExecutorEdit(QWidget *parent) : ISListEdit(parent)
{
	ButtonDesignateMe = new ISPushButton(this);
	ButtonDesignateMe->setText(LANG("Task.DesignateMe"));
	ButtonDesignateMe->setIcon(BUFFER_ICONS("User"));
	ButtonDesignateMe->setCursor(CURSOR_POINTING_HAND);
	ButtonDesignateMe->setSizePolicy(QSizePolicy::Maximum, ButtonDesignateMe->sizePolicy().verticalPolicy());
	connect(ButtonDesignateMe, &ISPushButton::clicked, this, &ISExecutorEdit::DesignateMe);
	AddWidgetToRight(ButtonDesignateMe);
}
//-----------------------------------------------------------------------------
ISExecutorEdit::~ISExecutorEdit()
{

}
//-----------------------------------------------------------------------------
void ISExecutorEdit::SetReadOnly(bool read_only)
{
	ISListEdit::SetReadOnly(read_only);
	ButtonDesignateMe->setVisible(!read_only);
}
//-----------------------------------------------------------------------------
void ISExecutorEdit::DesignateMe()
{
	SetValue(CURRENT_USER_ID);
}
//-----------------------------------------------------------------------------
