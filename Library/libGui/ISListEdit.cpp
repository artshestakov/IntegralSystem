#include "ISListEdit.h"
#include "ISConstants.h"
#include "ISListEditPopup.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISMetaDataHelper.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISUserRoleEntity.h"
#include "ISSystem.h"
#include "ISStyleSheet.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISListEdit::ISListEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	SetSizePolicyHorizontal(QSizePolicy::Minimum);
	CreateButtonClear();

	ButtonMain = new ISPushButton(this);
	ButtonMain->setText(LANG("NotSelected"));
	ButtonMain->setToolTip(LANG("ClickFromShowList"));
	ButtonMain->setIcon(BUFFER_ICONS("ArrowDown"));
	ButtonMain->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
	ButtonMain->setCursor(CURSOR_POINTING_HAND);
	ButtonMain->setStyleSheet(ButtonMain->styleSheet() + STYLE_SHEET("QPushButtonListEdit"));
	ButtonMain->setCheckable(true);
	connect(ButtonMain, &ISPushButton::clicked, this, &ISListEdit::ShowPopup);
	AddWidgetEdit(ButtonMain, this);

	ButtonList = new QToolButton(this);
	ButtonList->setFixedHeight(SIZE_MINIMUM_HEIGHT_EDIT_FIELD);
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
void ISListEdit::SetVisibleClear(bool Visible)
{
	GetButtonClear()->setVisible(Visible);
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
void ISListEdit::InvokeList(PMetaClassForeign *meta_foreign)
{
	MetaForeign = meta_foreign;
	MetaTable = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->GetForeignClass());

	ListEditPopup = new ISListEditPopup(MetaForeign, this);
	connect(ListEditPopup, &ISListEditPopup::Selected, this, &ISListEdit::SelectedValue);
	connect(ListEditPopup, &ISListEditPopup::Hided, this, &ISListEdit::HidedPopup);

	if (MetaTable->GetShowOnly())
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
	ListEditPopup->ShowAnimated(false, 300);
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
	int SelectedID = ISGui::SelectObject(MetaTable->GetName(), GetValue().toInt());
	if (SelectedID)
	{
		SetValue(SelectedID);
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::CreateObject()
{
	if (ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->GetUID(), CONST_UID_GROUP_ACCESS_TYPE_CREATE))
	{
		ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->GetName())->show();
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Create").arg(MetaTable->GetLocalListName()));
	}
}
//-----------------------------------------------------------------------------
void ISListEdit::EditObject()
{
	if (ISUserRoleEntity::GetInstance().CheckAccessTable(MetaTable->GetUID(), CONST_UID_GROUP_ACCESS_TYPE_EDIT))
	{
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, MetaTable->GetName(), GetValue().toInt())->show();
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Edit").arg(MetaTable->GetLocalListName()));
	}
	
}
//-----------------------------------------------------------------------------
