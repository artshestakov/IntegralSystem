#include "ISFavoritesForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISFavorites.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISMetaData.h"
#include "ISButtons.h"
#include "ISGui.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISFavoritesForm::ISFavoritesForm(QWidget *parent, const QString &table_name)
	: ISInterfaceForm(parent),
	TableName(table_name)
{
	setWindowTitle(table_name.isEmpty() ? LANG("ISFavoritesForm.Title") : LANG("ISFavoritesForm.Title.Table").arg(ISMetaData::Instance().GetMetaTable(TableName)->LocalListName));
	setWindowIcon(BUFFER_ICONS("Favorites"));
	resize(ISDefines::Gui::SIZE_640_480);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	QAction *ActionOpen = ToolBar->addAction(BUFFER_ICONS("Select"), LANG("ISFavoritesForm.Open"), this, &ISFavoritesForm::Open);
	ActionOpen->setEnabled(false);

	QAction *ActionClearFavorites = ToolBar->addAction(BUFFER_ICONS("Clear"), LANG("ISFavoritesForm.Clear"), this, &ISFavoritesForm::Clear);

	ListWidget = new ISListWidget(this);
	ListWidget->AddAction(ActionOpen, true);
	ListWidget->AddAction(ActionClearFavorites, false);
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISFavoritesForm::ListWidgetDoubleClicked);
	GetMainLayout()->addWidget(ListWidget);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	LabelRowCount = new QLabel(this);
	LayoutBottom->addWidget(LabelRowCount);

	LayoutBottom->addStretch();

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("ISFavoritesForm.Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISFavoritesForm::close);
	LayoutBottom->addWidget(ButtonClose);

	ISTcpQuery qGetFavoritesNames(API_GET_FAVORITE_NAMES);
	if (!TableName.isEmpty()) //Если таблица указана - подставляем её в запрос
	{
		qGetFavoritesNames.BindValue("TableName", TableName);
	}
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
		ActionClearFavorites->setEnabled(!NamesList.isEmpty());
		LabelRowCount->setText(LANG("ISFavoritesForm.RecordCount").arg(NamesList.size()));
	}
	else
	{
		ISMessageBox::ShowCritical(this, qGetFavoritesNames.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
ISFavoritesForm::~ISFavoritesForm()
{

}
//-----------------------------------------------------------------------------
void ISFavoritesForm::Open()
{
	if (ListWidget->currentItem())
	{
		ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, ListWidget->currentItem()->data(Qt::UserRole).toString(), ListWidget->currentItem()->data(Qt::UserRole * 2).toInt()));
	}
}
//-----------------------------------------------------------------------------
void ISFavoritesForm::Clear()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteFavorites")))
	{
		ISTcpQuery qFavoritesDelete(API_FAVORITES_DELETE);
		if (!TableName.isEmpty()) //Если таблица указана - подставляем её в запрос
		{
			qFavoritesDelete.BindValue("TableName", TableName);
		}
		if (qFavoritesDelete.Execute())
		{
			ISFavorites::Instance().DeleteAll();
			ListWidget->Clear();
			LabelRowCount->setText(LANG("ISFavoritesForm.RecordCount").arg(0));
		}
		else
		{
			ISMessageBox::ShowCritical(this, qFavoritesDelete.GetErrorString());
		}
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
	Open();
}
//-----------------------------------------------------------------------------
