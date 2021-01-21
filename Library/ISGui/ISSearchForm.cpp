#include "ISSearchForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISComboSearchWidgets.h"
#include "ISDelegates.h"
#include "ISMessageBox.h"
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
	TreeWidget->setFocusPolicy(Qt::NoFocus);
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
	Search();
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

	//Панель для кнопки добавления/удаления (нужна для правильного центрирования за счёт Layout)
	QWidget *Widget = new QWidget(TreeWidget);
	Widget->setLayout(new QVBoxLayout());
	Widget->layout()->setContentsMargins(0, 0, 25, 0);
	TreeWidget->setItemWidget(TreeWidgetItem, 0, Widget);

	ISServiceButton *ButtonAction = new ISServiceButton(ParentItem ? BUFFER_ICONS("Delete") : BUFFER_ICONS("Add"),
		ParentItem ? LANG("ISSearchForm.DeleteField") : LANG("ISSearchForm.AddField"), Widget);
	ParentItem ?
		connect(ButtonAction, &ISServiceButton::clicked, this, &ISSearchForm::DeleteClicked) :
		connect(ButtonAction, &ISServiceButton::clicked, this, &ISSearchForm::AddClicked);
	ButtonAction->setFlat(true);
	ButtonAction->setFocusPolicy(Qt::NoFocus);
	ButtonAction->setProperty("FieldName", MetaField->Name);
	Widget->layout()->addWidget(ButtonAction);

	if (!ParentItem)
	{
		//Заголовок поискового поля
		QLabel *LabelName = new QLabel(MetaField->LabelName + ':', TreeWidget);
		TreeWidget->setItemWidget(TreeWidgetItem, 1, LabelName);

		//Виджет с выбором оператора
		QString SearchOperatorWidget = MetaField->Type == ISNamespace::FT_Int && MetaField->Foreign
			? "ISComboSearchBase" :
			ISMetaData::Instance().GetType(MetaField->Type).SearchConditionWidget;
		ISComboSearchBase *ComboSearchOperator = ISAlgorithm::CreatePointer<ISComboSearchBase *>(SearchOperatorWidget, Q_ARG(QWidget *, TreeWidget));
		TreeWidget->setItemWidget(TreeWidgetItem, 2, ComboSearchOperator);
	}

	//Поле редактирования
	ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(TreeWidget, MetaField);
	FieldEditBase->SetSizePolicyHorizontal(QSizePolicy::Minimum);
	if (MetaField->Foreign) //Если поле является справочным - инициализируем его
	{
		dynamic_cast<ISListEdit*>(FieldEditBase)->InvokeList(MetaField->Foreign);
	}
	TreeWidget->setItemWidget(TreeWidgetItem, 3, FieldEditBase);

	Map.emplace(ButtonAction, TreeWidgetItem);

	if (ParentItem)
	{
		FieldEditBase->SetFocus();
	}
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
	TreeWidget->removeItemWidget(TreeWidgetItem, 0);
	TreeWidget->removeItemWidget(TreeWidgetItem, 3);

	//Удаляем элемент
	QTreeWidgetItem *ParentItem = TreeWidgetItem->parent();
	delete ParentItem->takeChild(ParentItem->indexOfChild(TreeWidgetItem));

	//Удаляем элемент из массива
	std::map<QObject*, QTreeWidgetItem*>::iterator It = Map.find(sender());
	if (It != Map.end())
	{
		Map.erase(It);
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::Search()
{
	QVariantList VariantList;
	for (const auto &MapItem : Map)
	{
		//Получаем имя поля
		QString FieldName = MapItem.first->property("FieldName").toString();

		//Получаем виджеты
		QTreeWidgetItem *TreeWidgetItem = MapItem.second;
		ISComboSearchBase *ComboSearchBase = dynamic_cast<ISComboSearchBase *>(TreeWidget->itemWidget(TreeWidgetItem, 2));
		ISFieldEditBase *FieldEditBase = dynamic_cast<ISFieldEditBase *>(TreeWidget->itemWidget(TreeWidgetItem, 3));

		//Если поле не изменялось - пропускаем его
		QVariant Value = FieldEditBase->GetValue();
		if (!FieldEditBase->GetModificationFlag() || !Value.isValid())
		{
			continue;
		}

		if (ExistField(VariantList, FieldName)) //Если поле в списке уже есть, добавляем к нему значение
		{
			for (int i = 0; i < VariantList.size(); ++i)
			{
				QVariantMap VariantMap = VariantList[i].toMap();
				if (VariantMap["FieldName"].toString() == FieldName)
				{
					QVariantList Values = VariantMap["Values"].toList();
					Values.append(Value);
					VariantMap["Values"] = Values;
					VariantList[i] = VariantMap;
					break;
				}
			}
		}
		else //Поля в списке нет - добавляем
		{
			VariantList.append(QVariantMap
			{
				{ "FieldName", FieldName },
				{ "Operator", ComboSearchBase->GetOperator() },
				{ "Values", QVariantList() << Value }
			});
		}
	}

	if (!VariantList.isEmpty()) //Если значения для поиска есть: виджеты-редакторы заполнена
	{
		emit Search(VariantList);
		hide();
	}
	else //Виджеты-редакторы не заполнены
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SearchFieldsIsEmpty"));
	}
}
//-----------------------------------------------------------------------------
bool ISSearchForm::ExistField(const QVariantList &VariantList, const QString &FieldName)
{
	bool Result = false;
	for (const QVariant &Variant : VariantList)
	{
		QVariantMap VariantMap = Variant.toMap();
		Result = VariantMap["FieldName"].toString() == FieldName;
		if (Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
