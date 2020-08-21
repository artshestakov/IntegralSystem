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
//-----------------------------------------------------------------------------
ISFavoritesForm::ISFavoritesForm(QWidget *parent, const QString &table_name)
	: ISInterfaceForm(parent),
	TableName(table_name)
{
	setWindowTitle(table_name.isEmpty() ? LANG("Favorites") : LANG("FavoritesForTable").arg(ISMetaData::Instance().GetMetaTable(TableName)->LocalListName));
	setWindowIcon(BUFFER_ICONS("Favorites"));
	resize(ISDefines::Gui::SIZE_640_480);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

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

	LoadFavorites();

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISFavoritesForm::close);
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
	std::map<QString, ISVectorInt> Map;
	if (TableName.isEmpty()) //���������� ������� �� ������� - ���������� ��� �������
	{
		Map = ISFavorites::Instance().GetObjects();
	}
	else //������� ���������� ������� - ���������� ��������� ������� ������ �� ���
	{
		Map[TableName] = ISFavorites::Instance().GetObjects(TableName);
	}

	for (const auto &MapItem : Map)
	{
		for (int ObjectID : MapItem.second)
		{
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(ISMetaData::Instance().GetMetaTable(MapItem.first)->LocalListName + ": " + ISCore::GetObjectName(MapItem.first, ObjectID));
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ListWidgetItem->setData(Qt::UserRole, MapItem.first);
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
void ISFavoritesForm::ItemClicked(QListWidgetItem *Item)
{
    Q_UNUSED(Item);
	ActionOpen->setEnabled(true);
	ActionDelete->setEnabled(true);
}
//-----------------------------------------------------------------------------
