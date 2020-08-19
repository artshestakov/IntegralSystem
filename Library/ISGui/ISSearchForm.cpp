#include "ISSearchForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISListEdit.h"
#include "ISButtons.h"
#include "ISDelegates.h"
#include "ISMetaData.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISSearchForm::ISSearchForm(PMetaTable *meta_table, QWidget *parent) : ISInterfaceForm(parent)
{
	MetaTable = meta_table;

	setWindowTitle(LANG("Search.Advanced") + " - " + MetaTable->LocalListName);
	setWindowIcon(BUFFER_ICONS("Search"));
	resize(800, 600);

	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelTitle = new QLabel(this);
	LabelTitle->setText(LANG("Search.Advanced"));
	LabelTitle->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LabelTitle->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	GetMainLayout()->addWidget(LabelTitle);

	QStringList HeaderLabels;
	HeaderLabels.append(LANG("Search.Title.Field"));
	HeaderLabels.append(LANG("Search.Title.Conditiion"));
	HeaderLabels.append(LANG("Search.Title.Value"));
	HeaderLabels.append(QString());

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setHeaderLabels(HeaderLabels);
	TreeWidget->setDropIndicatorShown(false);
	TreeWidget->setAlternatingRowColors(true);
	TreeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidget->setAnimated(true);
	TreeWidget->setItemDelegate(new ISDelegateSearchField(TreeWidget));
	GetMainLayout()->addWidget(TreeWidget);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	ButtonAddFilter = new ISPushButton(this);
	ButtonAddFilter->setText(LANG("Search.AddFilter"));
	ButtonAddFilter->setIcon(BUFFER_ICONS("Add"));
	ButtonAddFilter->setCursor(CURSOR_POINTING_HAND);
	ButtonAddFilter->setMenu(new QMenu(ButtonAddFilter));
	LayoutBottom->addWidget(ButtonAddFilter);

	ButtonClear = new ISPushButton(this);
	ButtonClear->setEnabled(false);
	ButtonClear->setText(LANG("Clear"));
	connect(ButtonClear, &ISPushButton::clicked, this, &ISSearchForm::DeleteAllFields);
	LayoutBottom->addWidget(ButtonClear);

	LayoutBottom->addStretch();

	ButtonSearch = new ISPushButton(this);
	ButtonSearch->setEnabled(false);
	ButtonSearch->setText(LANG("Search"));
	ButtonSearch->setIcon(BUFFER_ICONS("Search"));
	connect(ButtonSearch, &ISPushButton::clicked, this, &ISSearchForm::Search);
	LayoutBottom->addWidget(ButtonSearch);

	ButtonHide = new ISPushButton(this);
	ButtonHide->setText(LANG("Search.HideForm"));
	connect(ButtonHide, &ISPushButton::clicked, this, &ISSearchForm::hide);
	LayoutBottom->addWidget(ButtonHide);

	LoadFields();
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
	if (ButtonSearch->isEnabled())
	{
		Search();
	}
}
//-----------------------------------------------------------------------------
bool ISSearchForm::CheckExistField(const QString &FieldName) const
{
	bool Result = false;
	for (int i = 0; i < TreeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *TreeWidgetItem = TreeWidget->topLevelItem(i);
		if (TreeWidgetItem->childCount())
		{
			for (int j = 0; j < TreeWidgetItem->childCount(); ++j)
			{
				if (TreeWidgetItem->child(j)->data(0, Qt::UserRole).toString() == FieldName)
				{
					Result = true;
					break;
				}
			}
		}
		else
		{
			if (TreeWidgetItem->data(0, Qt::UserRole).toString() == FieldName)
			{
				Result = true;
				break;
			}
		}

		if (Result)
		{
			break;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISSearchForm::LoadFields()
{
	AddFieldFromList(MetaTable->GetFieldID()->LabelName, MetaTable->GetFieldID()->Name);

	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (MetaField->HideFromList || MetaField->NotSearch || !MetaField->QueryText.isEmpty())
		{
			continue;
		}

		if (ISMetaData::Instance().GetSearch(MetaField->Type))
		{
			AddFieldFromList(MetaField->LabelName, MetaField->Name);
		}
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddFieldFromList(const QString &FieldLocalName, const QString &FieldName)
{
	QAction *ActionFilter = new QAction(ButtonAddFilter->menu());
	ActionFilter->setText(FieldLocalName);
	ActionFilter->setData(FieldName);
	connect(ActionFilter, &QAction::triggered, this, &ISSearchForm::AddFilter);
	ButtonAddFilter->menu()->addAction(ActionFilter);
}
//-----------------------------------------------------------------------------
void ISSearchForm::AddFilter()
{
	QString FieldName = dynamic_cast<QAction*>(sender())->data().toString();
	PMetaField *MetaField = MetaTable->GetField(FieldName);
	QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem();
	TreeWidgetItem->setData(0, Qt::UserRole, FieldName);

	if (CheckExistField(FieldName)) //Если такое поле уже есть побавить child
	{
		QTreeWidgetItem *TopLevelItem = nullptr;
		for (int i = 0; i < TreeWidget->topLevelItemCount(); ++i)
		{
			if (TreeWidget->topLevelItem(i)->data(0, Qt::UserRole).toString() == FieldName)
			{
				TopLevelItem = TreeWidget->topLevelItem(i);
				break;
			}
		}
		
		TreeWidgetItem->setText(0, LANG("Search.Or"));
		TreeWidgetItem->setTextAlignment(0, Qt::AlignVCenter | Qt::AlignRight);
		TopLevelItem->addChild(TreeWidgetItem);
	}
	else //Поле верхнего уровня
	{
		TreeWidgetItem->setText(0, MetaField->LabelName);
		TreeWidget->addTopLevelItem(TreeWidgetItem);
	}

	ISComboSearchBase *ComboSearchBase = ISGui::CreateSearchOperator(TreeWidget, MetaField->Type, MetaField->Foreign);
	TreeWidget->setItemWidget(TreeWidgetItem, 1, ComboSearchBase);

	ISFieldEditBase *FieldEditBase = ISGui::CreateColumnForField(TreeWidget, MetaField);
	TreeWidget->setItemWidget(TreeWidgetItem, 2, FieldEditBase);

	ISListEdit *ListEdit = dynamic_cast<ISListEdit*>(FieldEditBase);
	if (ListEdit)
	{
		ListEdit->InvokeList(MetaField->Foreign);
	}

	QHBoxLayout *LayoutWidget = new QHBoxLayout();
	QWidget *Widget = new QWidget();
	Widget->setLayout(LayoutWidget);

	ISServiceButton *ButtonDelete = new ISServiceButton(BUFFER_ICONS("DeleteSearchField"), LANG("Search.DeleteField"), Widget);
	connect(ButtonDelete, &ISPushButton::clicked, this, &ISSearchForm::DeleteFieldClicked);
	LayoutWidget->addWidget(ButtonDelete);
	LayoutWidget->addStretch();
	TreeWidget->setItemWidget(TreeWidgetItem, 3, Widget);
	MapItems.insert(ButtonDelete, TreeWidgetItem);
	
	QTreeWidgetItem *ParentItem = TreeWidgetItem->parent();
	if (ParentItem)
	{
		if (!ParentItem->isExpanded())
		{
			QTimer::singleShot(150, this, [ParentItem] () { ParentItem->setExpanded(true); });
		}
	}

	TreeWidget->resizeColumnToContents(2);
	TreeWidget->repaint();
	UpdateInterface();
	FieldEditBase->SetFocus();
}
//-----------------------------------------------------------------------------
void ISSearchForm::Search()
{
	ISSearchModel SearchModel;
	for (int i = 0; i < TreeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *TreeWidgetItem = TreeWidget->topLevelItem(i);
		if (TreeWidgetItem->childCount())
		{
			SearchFromField(SearchModel, TreeWidgetItem);
			for (int j = 0; j < TreeWidgetItem->childCount(); ++j)
			{
				SearchFromField(SearchModel, TreeWidgetItem->child(j));
			}
		}
		else
		{
			SearchFromField(SearchModel, TreeWidgetItem);
		}
	}

	QString SearchString;
	QVariantMap VariantMap;
	SearchModel.CreateSearchString(SearchString, VariantMap);
	if (VariantMap.count())
	{
		emit StartSearch(SearchString, VariantMap);
		hide();
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::SearchFromField(ISSearchModel &SearchModel, QTreeWidgetItem *TreeWidgetItem)
{
	PMetaField *MetaField = MetaTable->GetField(TreeWidgetItem->data(0, Qt::UserRole).toString());
	QString FieldName = MetaTable->Alias + '_' + MetaField->Name;
	QString Condition = ':' + MetaField->Name;

	ISComboSearchBase *ComboSearchBase = dynamic_cast<ISComboSearchBase*>(TreeWidget->itemWidget(TreeWidgetItem, 1));
	ISNamespace::SearchOperatorType SearchOperator = ComboSearchBase->GetOperator();

	ISFieldEditBase *FieldEditBase = dynamic_cast<ISFieldEditBase*>(TreeWidget->itemWidget(TreeWidgetItem, 2));
	QVariant Value = FieldEditBase->GetValue().toString().toLower();

	if (Value.isValid())
	{
		SearchModel.AddField(FieldName, Value, SearchOperator, MetaField->Type);
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.SearchFieldNotValid").arg(MetaField->LabelName));
		FieldEditBase->BlinkRed();
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::DeleteFieldClicked()
{
	ISServiceButton *ButtonDelete = dynamic_cast<ISServiceButton*>(sender());
	DeleteField(MapItems.value(ButtonDelete));
}
//-----------------------------------------------------------------------------
void ISSearchForm::DeleteAllFields()
{
	while (TreeWidget->topLevelItemCount())
	{
		DeleteField(TreeWidget->topLevelItem(0));
	}
}
//-----------------------------------------------------------------------------
void ISSearchForm::DeleteField(QTreeWidgetItem *TreeWidgetItem)
{
	if (TreeWidgetItem->childCount())
	{
		while (TreeWidgetItem->childCount()) //Обход всех дочерних элементов
		{
			QTreeWidgetItem *Temp = TreeWidgetItem->child(0);
			DeleteField(Temp);
		}
		DeleteField(TreeWidgetItem);
	}
	else
	{
		for (int i = 0; i < TreeWidget->columnCount(); ++i) //Обход всех виджетом итема
		{
			QWidget *ItemWidget = TreeWidget->itemWidget(TreeWidgetItem, i);
			if (ItemWidget) //Если виджет у итема имеется - удалить его
			{
				TreeWidget->setItemWidget(TreeWidgetItem, i, nullptr);
				delete ItemWidget;
				ItemWidget = nullptr;
			}
		}

		QTreeWidgetItem *Temp = nullptr;
		if (TreeWidgetItem->parent())
		{
			Temp = TreeWidgetItem->parent()->takeChild(TreeWidgetItem->parent()->indexOfChild(TreeWidgetItem));
		}
		else
		{
			Temp = TreeWidget->takeTopLevelItem(TreeWidget->indexOfTopLevelItem(TreeWidgetItem));
		}
		delete Temp;
	}

	UpdateInterface();
}
//-----------------------------------------------------------------------------
void ISSearchForm::UpdateInterface()
{
	ButtonClear->setEnabled(TreeWidget->topLevelItemCount());
	ButtonSearch->setEnabled(TreeWidget->topLevelItemCount());
}
//-----------------------------------------------------------------------------
