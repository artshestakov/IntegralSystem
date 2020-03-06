#include "ISExportForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportForm::ISExportForm(PMetaClassTable *meta_table, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	MetaTable = meta_table;
	SelectedType = ISNamespace::ET_Unknown;

	setWindowIcon(BUFFER_ICONS("ExportTable"));
	setWindowTitle(LANG("Export.Table"));

	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelText = new QLabel(this);
	LabelText->setText(LANG("SelectExportType") + ':');
	GetMainLayout()->addWidget(LabelText);

	ComboBoxType = new ISComboEdit(this);
	ComboBoxType->SetEditable(false);
	ComboBoxType->AddItem(LANG("NotSelected"), ISNamespace::ET_Unknown);
	ComboBoxType->AddItem(LANG("Export.Type.CSV"), ISNamespace::ET_CSV);
	ComboBoxType->AddItem(LANG("Export.Type.HTML"), ISNamespace::ET_HTML);
	ComboBoxType->AddItem(LANG("Export.Type.DBF"), ISNamespace::ET_DBF);
	ComboBoxType->AddItem(LANG("Export.Type.XML"), ISNamespace::ET_XML);
	GetMainLayout()->addWidget(ComboBoxType);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);

	CreateTabSettings();
	CreateTabFields();

	ButtonPanel = new ISButtonDialog(this, LANG("Export"));
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
ISVectorString ISExportForm::GetSelectedFields() const
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
	TabWidget->addTab(TabSettings, LANG("Export.Settings"));

	CheckHeader = new ISCheckEdit(TabSettings);
	CheckHeader->SetValue(true);
	FormLayout->addRow(LANG("Export.Setting.Header") + ':', CheckHeader);
}
//-----------------------------------------------------------------------------
void ISExportForm::CreateTabFields()
{
	QVBoxLayout *LayoutFields = new QVBoxLayout();

	QWidget *TabFields = new QWidget(TabWidget);
	TabFields->setLayout(LayoutFields);
	TabWidget->addTab(TabFields, LANG("Export.Fields"));

	QToolBar *ToolBar = new QToolBar(TabFields);
	LayoutFields->addWidget(ToolBar);

	QAction *ActionSelectAll = new QAction(ToolBar);
	ActionSelectAll->setText(LANG("SelectAllFields"));
	ActionSelectAll->setToolTip(LANG("SelectAllFields"));
	ActionSelectAll->setIcon(BUFFER_ICONS("CheckAll"));
	ToolBar->addAction(ActionSelectAll);
	connect(ActionSelectAll, &QAction::triggered, [=]
	{
		for (int i = 0; i < ListFields->count(); ++i)
		{
			ListFields->item(i)->setCheckState(Qt::Checked);
		}
	});

	QAction *ActionDeselectAll = new QAction(ToolBar);
	ActionDeselectAll->setText(LANG("DeselectAllFields"));
	ActionDeselectAll->setToolTip(LANG("DeselectAllFields"));
	ActionDeselectAll->setIcon(BUFFER_ICONS("CheckNotAll"));
	ToolBar->addAction(ActionDeselectAll);
	connect(ActionDeselectAll, &QAction::triggered, [=]
	{
		for (int i = 0; i < ListFields->count(); ++i)
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

	for (int i = 0; i < MetaTable->Fields.count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->Fields[i];
		if (!MetaField->HideFromList)
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
		ISMessageBox::ShowWarning(this, LANG("Export.UnknownType"));
		return;
	}

	if (SelectedFields.empty())
	{
		ISMessageBox::ShowWarning(this, LANG("Export.NotSelectedFields"));
		return;
	}

	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISExportForm::CreateFieldItem(PMetaClassField *MetaField)
{
	QListWidgetItem *FieldItem = new QListWidgetItem(ListFields);
	FieldItem->setText(MetaField->LabelName);
	FieldItem->setData(Qt::UserRole, MetaField->Name);
	FieldItem->setCheckState(Qt::Checked);
	FieldItem->setSizeHint(QSize(FieldItem->sizeHint().width(), 30));
	SelectedFields.emplace_back(MetaField->Name);
}
//-----------------------------------------------------------------------------
void ISExportForm::FieldsPositionChanged()
{
	SelectedFields.clear();
	for (int i = 0; i < ListFields->count(); ++i)
	{
		QListWidgetItem *Item = ListFields->item(i);
		if (Item->text().length())
		{
			if (Item->checkState() == Qt::Checked)
			{
				SelectedFields.emplace_back(Item->data(Qt::UserRole).toString());
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
