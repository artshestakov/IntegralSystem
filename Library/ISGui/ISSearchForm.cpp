#include "ISSearchForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISComboSearchWidgets.h"

#include "ISDelegates.h"
//-----------------------------------------------------------------------------
ISSearchForm::ISSearchForm(PMetaTable *meta_table, QWidget *parent)
	: ISInterfaceForm(parent),
	MetaTable(meta_table)
{
	setWindowTitle(LANG("ISSearchForm.Title") + " - " + MetaTable->LocalListName);
	setWindowIcon(BUFFER_ICONS("Search"));
	resize(800, 600);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setHeaderLabels(QStringList() << "Field name" << "Operator" << "Edit" << "Button");
	TreeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidget->setAnimated(true);
	TreeWidget->setSelectionMode(QAbstractItemView::NoSelection);
	TreeWidget->setHeaderHidden(true);
	TreeWidget->setItemDelegate(new ISDelegateSearchField(TreeWidget));
	TreeWidget->setFrameShape(QFrame::NoFrame);
	GetMainLayout()->addWidget(TreeWidget);

	for (PMetaField *MetaField : MetaTable->Fields)
	{
		//Пропускаем системные поля и поля по которым поиск зарещён
		if (MetaField->IsSystem || MetaField->NotSearch)
		{
			continue;
		}
		AddField(MetaField);
	}

	for (int i = 0; i < TreeWidget->columnCount(); ++i)
	{
		TreeWidget->resizeColumnToContents(i);
	}

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->addStretch();
	GetMainLayout()->addLayout(LayoutBottom);

	ButtonSearch = new ISPushButton(this);
	ButtonSearch->setEnabled(false);
	ButtonSearch->setText(LANG("Search"));
	ButtonSearch->setIcon(BUFFER_ICONS("Search"));
	connect(ButtonSearch, &ISPushButton::clicked, this, static_cast<void(ISSearchForm::*)(void)>(&ISSearchForm::Search));
	LayoutBottom->addWidget(ButtonSearch);

	ButtonHide = new ISPushButton(this);
	ButtonHide->setText(LANG("Search.HideForm"));
	connect(ButtonHide, &ISPushButton::clicked, this, &ISSearchForm::hide);
	LayoutBottom->addWidget(ButtonHide);
}
//-----------------------------------------------------------------------------
ISSearchForm::~ISSearchForm()
{

}
//-----------------------------------------------------------------------------
void ISSearchForm::closeEvent(QCloseEvent *e)
{
	e->ignore();
	hide();
}
//-----------------------------------------------------------------------------
void ISSearchForm::EscapeClicked()
{
	hide();
}
//-----------------------------------------------------------------------------
void ISSearchForm::EnterClicked()
{
	
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddField(PMetaField *MetaField, QTreeWidgetItem *ParentItem)
{
	QTreeWidgetItem *TreeWidgetItem = nullptr;
	if (ParentItem)
	{
		TreeWidgetItem = new QTreeWidgetItem(ParentItem);
		TreeWidget->expandItem(ParentItem);
	}
	else
	{
		TreeWidgetItem = new QTreeWidgetItem(TreeWidget);
	}

	if (!ParentItem)
	{
		//Заголовок поискового поля
		QLabel *LabelName = new QLabel(MetaField->LabelName + ':', TreeWidget);
		LabelName->setContentsMargins(0, 0, 25, 0);
		TreeWidget->setItemWidget(TreeWidgetItem, 0, LabelName);

		//Виджет с выбором оператора
		QString SearchOperatorWidget = MetaField->Type == ISNamespace::FT_Int && MetaField->Foreign
			? "ISComboSearchBase" :
			ISMetaData::Instance().GetSearchWidget(MetaField->Type);
		ISComboSearchBase *ComboSearchOperator = ISAlgorithm::CreatePointer<ISComboSearchBase *>(SearchOperatorWidget, Q_ARG(QWidget *, TreeWidget));
		TreeWidget->setItemWidget(TreeWidgetItem, 1, ComboSearchOperator);
	}

	//Поле редактирования
	ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(TreeWidget, MetaField);
	FieldEditBase->SetSizePolicyHorizontal(QSizePolicy::Minimum);
	if (MetaField->Foreign) //Если поле является справочным - инициализируем его
	{
		dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
	}
	TreeWidget->setItemWidget(TreeWidgetItem, 2, FieldEditBase);

	//Панель для кнопки добавления/удаления (нужна для правильного центрирования за счёт Layout)
	QWidget *Widget = new QWidget(TreeWidget);
	Widget->setLayout(new QVBoxLayout());
	TreeWidget->setItemWidget(TreeWidgetItem, 3, Widget);

	ISServiceButton *ButtonAction = new ISServiceButton(ParentItem ? BUFFER_ICONS("Delete") : BUFFER_ICONS("Add"),
		ParentItem ? LANG("ISSearchForm.DeleteField") : LANG("ISSearchForm.AddField"), Widget);
	ParentItem ?
		connect(ButtonAction, &ISServiceButton::clicked, this, &ISSearchForm::DeleteClicked) :
		connect(ButtonAction, &ISServiceButton::clicked, this, &ISSearchForm::AddClicked);
	ButtonAction->setFlat(true);
	ButtonAction->setProperty("FieldName", ParentItem ? QVariant() : MetaField->Name);
	Widget->layout()->addWidget(ButtonAction);

	Map.emplace(ButtonAction, TreeWidgetItem);
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddClicked()
{
	PMetaField *MetaField = MetaTable->GetField(sender()->property("FieldName").toString());
	AddField(MetaField, Map[sender()]);
}
//-----------------------------------------------------------------------------
void ISSearchForm::DeleteClicked()
{
	//Получаем элементы
	QTreeWidgetItem *TreeWidgetItem = Map[sender()];

	//Удаляем виджеты
	TreeWidget->removeItemWidget(TreeWidgetItem, 2);
	TreeWidget->removeItemWidget(TreeWidgetItem, 3);

	//Удаляем элемент
	QTreeWidgetItem *ParentItem = TreeWidgetItem->parent();
	delete ParentItem->takeChild(ParentItem->indexOfChild(TreeWidgetItem));

	std::map<QObject*, QTreeWidgetItem*>::iterator It = Map.find(sender());
	if (It != Map.end())
	{
		Map.erase(It);
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::Search()
{
	
}
//-----------------------------------------------------------------------------
