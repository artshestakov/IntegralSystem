#include "ISFavoritesForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISFavorites.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISButtons.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISFavoritesForm::ISFavoritesForm(QWidget *parent, const QString &table_name)
	: ISInterfaceForm(parent),
	TableName(table_name)
{
	setWindowTitle(table_name.isEmpty() ? LANG("Favorites") : LANG("FavoritesForTable").arg(ISMetaData::Instance().GetMetaTable(TableName)->LocalListName));
	setWindowIcon(BUFFER_ICONS("Favorites"));
	resize(ISDefines::Gui::SIZE_640_480);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	QAction *ActionOpen = new QAction(ToolBar);
	ActionOpen->setText(LANG("Open"));
	ActionOpen->setToolTip(LANG("Open"));
	ActionOpen->setIcon(BUFFER_ICONS("Select"));
	ActionOpen->setEnabled(false);
	connect(ActionOpen, &QAction::triggered, this, &ISFavoritesForm::OpenFavorite);
	ToolBar->addAction(ActionOpen);

	QAction *ActionDelete = new QAction(ToolBar);
	ActionDelete->setText(LANG("Delete"));
	ActionDelete->setToolTip(LANG("Delete"));
	ActionDelete->setIcon(BUFFER_ICONS("Delete"));
	ActionDelete->setEnabled(false);
	connect(ActionDelete, &QAction::triggered, this, &ISFavoritesForm::DeleteFavorite);
	ToolBar->addAction(ActionDelete);

	QAction *ActionClearFavorites = new QAction(ToolBar);
	ActionClearFavorites->setText(LANG("ClearFavorites"));
	ActionClearFavorites->setToolTip(LANG("ClearFavorites"));
	ActionClearFavorites->setIcon(BUFFER_ICONS("Clear"));
	connect(ActionClearFavorites, &QAction::triggered, this, &ISFavoritesForm::ClearFavorites);
	ToolBar->addAction(ActionClearFavorites);

	ListWidget = new ISListWidget(this);
	ListWidget->AddAction(ActionOpen, true);
	ListWidget->AddAction(ActionDelete, true);
	ListWidget->AddAction(ActionClearFavorites, false);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISFavoritesForm::ListWidgetDoubleClicked);
	GetMainLayout()->addWidget(ListWidget);

	LoadFavorites();

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISFavoritesForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISFavoritesForm::~ISFavoritesForm()
{

}
//-----------------------------------------------------------------------------
void ISFavoritesForm::LoadFavorites()
{
	ListWidget->Clear();
	ISTcpQuery qGetFavoritesNames(API_GET_FAVORITE_NAMES);
	if (qGetFavoritesNames.Execute())
	{
		QVariantList NamesList = qGetFavoritesNames.TakeAnswer()["Names"].toList();
		for (const QVariant &Variant : NamesList)
		{
			QVariantMap NameMap = Variant.toMap();
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(NameMap["TableLocalName"].toString() + ": " + NameMap["ObjectName"].toString());
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setData(Qt::UserRole, NameMap["TableName"].toString());
			ListWidgetItem->setData(Qt::UserRole * 2, NameMap["ObjectID"].toUInt());
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetFavoritesNames.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::ReloadFavorites()
{
	ListWidget->Clear();
	LoadFavorites();
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::OpenFavorite()
{
	if (ListWidget->currentItem())
	{
		ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, ListWidget->currentItem()->data(Qt::UserRole).toString(), ListWidget->currentItem()->data(Qt::UserRole * 2).toInt()));
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::DeleteFavorite()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteFavorite")))
	{
		if (ListWidget->currentItem())
		{
			ISFavorites::Instance().DeleteFavorite(ListWidget->currentItem()->data(Qt::UserRole).toString(), ListWidget->currentItem()->data(Qt::UserRole * 2).toInt());
			ReloadFavorites();
		}
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::ClearFavorites()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteFavorites")))
	{
		ISFavorites::Instance().DeleteAllFavorites();
		ReloadFavorites();
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::ListWidgetDoubleClicked(QListWidgetItem *Item)
{
    Q_UNUSED(Item);
	OpenFavorite();
}
//-----------------------------------------------------------------------------
