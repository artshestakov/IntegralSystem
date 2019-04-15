#include "StdAfx.h"
#include "ISExportForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportForm::ISExportForm(PMetaClassTable *meta_table, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	MetaTable = meta_table;
	SelectedType = ISNamespace::ET_Unknown;

	setWindowIcon(BUFFER_ICONS("ExportTable"));
	setWindowTitle(LOCALIZATION("Export.Table"));

	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QLabel *LabelText = new QLabel(this);
	LabelText->setText(LOCALIZATION("SelectExportType") + ":");
	GetMainLayout()->addWidget(LabelText);

	ComboBoxType = new ISComboEdit(this);
	ComboBoxType->SetEditable(false);
	ComboBoxType->AddItem(LOCALIZATION("NotSelected"), ISNamespace::ET_Unknown);
	ComboBoxType->AddItem(LOCALIZATION("Export.Type.CSV"), ISNamespace::ET_CSV);
	ComboBoxType->AddItem(LOCALIZATION("Export.Type.HTML"), ISNamespace::ET_HTML);
	ComboBoxType->AddItem(LOCALIZATION("Export.Type.DBF"), ISNamespace::ET_DBF);
	ComboBoxType->AddItem(LOCALIZATION("Export.Type.XML"), ISNamespace::ET_XML);
	GetMainLayout()->addWidget(ComboBoxType);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);

	CreateTabSettings();
	CreateTabFields();

	ButtonPanel = new ISButtonDialog(this, LOCALIZATION("Export"));
	connect(ButtonPanel, &ISButtonDialog::Apply, this, &ISExportForm::Select);
	connect(ButtonPanel, &ISButtonDialog::Close, this, &ISExportForm::close);
	GetMainLayout()->addWidget(ButtonPanel);
}
//-----------------------------------------------------------------------------
ISExportForm::~ISExportForm()
{

}
//-----------------------------------------------------------------------------
ISNamespace::ExportType ISExportForm::GetSelectedType()
{
	return SelectedType;
}
//-----------------------------------------------------------------------------
QString ISExportForm::GetSelectTypeName() const
{
	return ComboBoxType->GetCurrentText();
}
//-----------------------------------------------------------------------------
QList<QString> ISExportForm::GetSelectedFields() const
{
	return SelectedFields;
}
//-----------------------------------------------------------------------------
bool ISExportForm::GetHeader() const
{
	return CheckHeader->GetValue().toBool();
}
//-----------------------------------------------------------------------------
void ISExportForm::CreateTabSettings()
{
	QFormLayout *FormLayout = new QFormLayout();

	QWidget *TabSettings = new QWidget(TabWidget);
	TabSettings->setLayout(FormLayout);
	TabWidget->addTab(TabSettings, LOCALIZATION("Export.Settings"));

	CheckHeader = new ISCheckEdit(TabSettings);
	CheckHeader->SetValue(true);
	FormLayout->addRow(LOCALIZATION("Export.Setting.Header") + ":", CheckHeader);
}
//-----------------------------------------------------------------------------
void ISExportForm::CreateTabFields()
{
	QVBoxLayout *LayoutFields = new QVBoxLayout();

	QWidget *TabFields = new QWidget(TabWidget);
	TabFields->setLayout(LayoutFields);
	TabWidget->addTab(TabFields, LOCALIZATION("Export.Fields"));

	QToolBar *ToolBar = new QToolBar(TabFields);
	LayoutFields->addWidget(ToolBar);

	QAction *ActionSelectAll = new QAction(ToolBar);
	ActionSelectAll->setText(LOCALIZATION("SelectAllFields"));
	ActionSelectAll->setToolTip(LOCALIZATION("SelectAllFields"));
	ActionSelectAll->setIcon(BUFFER_ICONS("CheckAll"));
	ToolBar->addAction(ActionSelectAll);
	connect(ActionSelectAll, &QAction::triggered, [=]
	{
		for (int i = 0; i < ListFields->count(); i++)
		{
			ListFields->item(i)->setCheckState(Qt::Checked);
		}
	});

	QAction *ActionDeselectAll = new QAction(ToolBar);
	ActionDeselectAll->setText(LOCALIZATION("DeselectAllFields"));
	ActionDeselectAll->setToolTip(LOCALIZATION("DeselectAllFields"));
	ActionDeselectAll->setIcon(BUFFER_ICONS("CheckNotAll"));
	ToolBar->addAction(ActionDeselectAll);
	connect(ActionDeselectAll, &QAction::triggered, [=]
	{
		for (int i = 0; i < ListFields->count(); i++)
		{
			ListFields->item(i)->setCheckState(Qt::Unchecked);
		}
	});

	ListFields = new ISListWidget(TabFields);
	ListFields->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ListFields->setDragEnabled(true);
	ListFields->setAcceptDrops(true);
	ListFields->setDropIndicatorShown(true);
	ListFields->setDefaultDropAction(Qt::MoveAction);
	LayoutFields->addWidget(ListFields);

	CreateFieldItem(MetaTable->GetFieldID());

	for (int i = 0; i < MetaTable->GetFields().count(); i++)
	{
		PMetaClassField *MetaField = MetaTable->GetFields().at(i);
		if (!MetaField->GetHideFromList())
		{
			CreateFieldItem(MetaField);
		}
	}

	connect(ListFields, &QListWidget::itemChanged, this, &ISExportForm::FieldsPositionChanged);
	connect(ListFields, &QListWidget::itemDoubleClicked, this, &ISExportForm::ItemDoubleClicked);
	connect(ListFields->model(), &QAbstractListModel::rowsMoved, this, &ISExportForm::FieldsPositionChanged);
}
//-----------------------------------------------------------------------------
void ISExportForm::Select()
{
	SelectedType = qvariant_cast<ISNamespace::ExportType>(ComboBoxType->GetValue());

	if (SelectedType == ISNamespace::ET_Unknown)
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Export.UnknownType"));
		return;
	}

	if (!SelectedFields.count())
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Export.NotSelectedFields"));
		return;
	}

	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISExportForm::CreateFieldItem(PMetaClassField *MetaField)
{
	QListWidgetItem *FieldItem = new QListWidgetItem(ListFields);
	FieldItem->setText(MetaField->GetLabelName());
	FieldItem->setData(Qt::UserRole, MetaField->GetName());
	FieldItem->setCheckState(Qt::Checked);
	FieldItem->setSizeHint(QSize(FieldItem->sizeHint().width(), 30));
	SelectedFields.append(MetaField->GetName());
}
//-----------------------------------------------------------------------------
void ISExportForm::FieldsPositionChanged()
{
	SelectedFields.clear();

	for (int i = 0; i < ListFields->count(); i++)
	{
		QListWidgetItem *Item = ListFields->item(i);
		if (Item->text().length())
		{
			if (Item->checkState() == Qt::Checked)
			{
				SelectedFields.append(Item->data(Qt::UserRole).toString());
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISExportForm::ItemDoubleClicked(QListWidgetItem *item)
{
	if (item->checkState() == Qt::Checked)
	{
		item->setCheckState(Qt::Unchecked);
	}
	else
	{
		item->setCheckState(Qt::Checked);
	}
}
//-----------------------------------------------------------------------------
void ISExportForm::EnterClicked()
{
	Select();
}
//-----------------------------------------------------------------------------
