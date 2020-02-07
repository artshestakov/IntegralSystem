#include "ISFavoritesForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISFavorites.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISQueryModel.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISButtons.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_FAVORITES = PREPARE_QUERY("SELECT fvts_id, fvts_tablename, fvts_tablelocalname, fvts_objectname, fvts_objectid "
											"FROM _favorites "
											"WHERE fvts_user = currentuserid() "
											"ORDER BY fvts_id DESC");
//-----------------------------------------------------------------------------
static QString QS_FAVORITES_TABLE = PREPARE_QUERY("SELECT fvts_id, fvts_tablename, fvts_tablelocalname, fvts_objectname, fvts_objectid "
												  "FROM _favorites "
												  "WHERE fvts_user = currentuserid() "
												  "AND fvts_tablename = :TableName "
												  "ORDER BY fvts_id DESC");
//-----------------------------------------------------------------------------
ISFavoritesForm::ISFavoritesForm(QWidget *parent, PMetaClassTable *meta_table) : ISInterfaceForm(parent)
{
	MetaTable = meta_table;

	if (MetaTable)
	{
		setWindowTitle(LANG("FavoritesForTable") + ": " + MetaTable->GetLocalListName());
	}
	else
	{
		setWindowTitle(LANG("Favorites"));
	}

	setWindowIcon(BUFFER_ICONS("Favorites"));

	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	ActionOpen = new QAction(ToolBar);
	ActionOpen->setText(LANG("Open"));
	ActionOpen->setToolTip(LANG("Open"));
	ActionOpen->setIcon(BUFFER_ICONS("Select"));
	ActionOpen->setEnabled(false);
	connect(ActionOpen, &QAction::triggered, this, &ISFavoritesForm::OpenFavorite);
	ToolBar->addAction(ActionOpen);

	ActionDelete = new QAction(ToolBar);
	ActionDelete->setText(LANG("Delete"));
	ActionDelete->setToolTip(LANG("Delete"));
	ActionDelete->setIcon(BUFFER_ICONS("Delete"));
	ActionDelete->setEnabled(false);
	connect(ActionDelete, &QAction::triggered, this, &ISFavoritesForm::DeleteFavorite);
	ToolBar->addAction(ActionDelete);

	ActionClearFavorites = new QAction(ToolBar);
	ActionClearFavorites->setText(LANG("ClearFavorites"));
	ActionClearFavorites->setToolTip(LANG("ClearFavorites"));
	ActionClearFavorites->setIcon(BUFFER_ICONS("Clear"));
	connect(ActionClearFavorites, &QAction::triggered, this, &ISFavoritesForm::ClearFavorites);
	ToolBar->addAction(ActionClearFavorites);

	ListWidget = new ISListWidget(this);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISFavoritesForm::ListWidgetDoubleClicked);
	connect(ListWidget, &QListWidget::itemClicked, this, &ISFavoritesForm::ItemClicked);
	GetMainLayout()->addWidget(ListWidget);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISFavoritesForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);

	LoadFavorites();
}
//-----------------------------------------------------------------------------
ISFavoritesForm::~ISFavoritesForm()
{

}
//-----------------------------------------------------------------------------
void ISFavoritesForm::LoadFavorites()
{
	ISQuery qSelect;

	if (MetaTable)
	{
		qSelect = ISQuery(QS_FAVORITES_TABLE);
		qSelect.BindValue(":TableName", MetaTable->GetName());
	}
	else
	{
		qSelect = ISQuery(QS_FAVORITES);
	}

	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int FavoriteID = qSelect.ReadColumn("fvts_id").toInt();
			QString TableName = qSelect.ReadColumn("fvts_tablename").toString();
			QString TableLocalName = qSelect.ReadColumn("fvts_tablelocalname").toString();
			QString ObjectName = qSelect.ReadColumn("fvts_objectname").toString();
			int ObjectID = qSelect.ReadColumn("fvts_objectid").toInt();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(TableLocalName + ": " + ObjectName);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setData(Qt::UserRole, TableName);
			ListWidgetItem->setData(Qt::UserRole * 2, ObjectID);
		}
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
		emit OpenObject(ListWidget->currentItem()->data(Qt::UserRole).toString(), ListWidget->currentItem()->data(Qt::UserRole * 2).toInt());
		close();
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::DeleteFavorite()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteFavorite")))
	{
		if (ListWidget->currentItem())
		{
			if (ISFavorites::GetInstance().DeleteFavorite(ListWidget->currentItem()->data(Qt::UserRole).toString(), ListWidget->currentItem()->data(Qt::UserRole * 2).toInt()))
			{
				ReloadFavorites();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::ClearFavorites()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteFavorites")))
	{
		ISFavorites::GetInstance().DeleteAllFavorites();
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
void ISFavoritesForm::ItemClicked(QListWidgetItem *Item)
{
    Q_UNUSED(Item);
	ActionOpen->setEnabled(true);
	ActionDelete->setEnabled(true);
}
//-----------------------------------------------------------------------------
