#include "ISExportDialog.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportDialog::ISExportDialog(ISTcpModel *TcpModel)
	: ISInterfaceDialogForm(),
	SelectedType(ISNamespace::ET_Unknown)
{
	setWindowIcon(BUFFER_ICONS("ExportTable"));
	setWindowTitle(LANG("Export.Table"));
	setFixedSize(ISDefines::Gui::SIZE_640_480);
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
	ComboBoxType->AddItem(LANG("Export.Type.JSON"), ISNamespace::ET_JSON);
	ComboBoxType->AddItem(LANG("Export.Type.SQLITE"), ISNamespace::ET_SQLITE);
	connect(ComboBoxType, &ISComboEdit::ValueChange, this, &ISExportDialog::TypeChanged);
	GetMainLayout()->addWidget(ComboBoxType);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);

	CreateTabSettings();
	CreateTabFields(TcpModel);

	ButtonDialog = new ISButtonDialog(this, LANG("Export"));
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISExportDialog::Select);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISExportDialog::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISExportDialog::~ISExportDialog()
{

}
//-----------------------------------------------------------------------------
ISNamespace::ExportType ISExportDialog::GetSelectedType()
{
	return SelectedType;
}
//-----------------------------------------------------------------------------
QString ISExportDialog::GetSelectTypeName() const
{
	return ComboBoxType->GetCurrentText();
}
//-----------------------------------------------------------------------------
ISVectorUInt ISExportDialog::GetSelectedFields() const
{
	return SelectedFields;
}
//-----------------------------------------------------------------------------
bool ISExportDialog::GetHeader() const
{
	return CheckHeader->GetValue().toBool();
}
//-----------------------------------------------------------------------------
void ISExportDialog::CreateTabSettings()
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
void ISExportDialog::CreateTabFields(ISTcpModel *TcpModel)
{
	QVBoxLayout *LayoutFields = new QVBoxLayout();

	QWidget *TabFields = new QWidget(TabWidget);
	TabFields->setLayout(LayoutFields);
	TabWidget->addTab(TabFields, LANG("Export.Fields"));

	QToolBar *ToolBar = new QToolBar(TabFields);
	ToolBar->setIconSize(ISDefines::Gui::SIZE_22_22);
	LayoutFields->addWidget(ToolBar);

	QAction *ActionSelectAll = new QAction(BUFFER_ICONS("CheckAll"), LANG("SelectAllFields"), ToolBar);
	connect(ActionSelectAll, &QAction::triggered, [=] { ListFields->SetCheckedItems(true); });
	ToolBar->addAction(ActionSelectAll);

	QAction *ActionDeselectAll = new QAction(BUFFER_ICONS("CheckNotAll"), LANG("DeselectAllFields"), ToolBar);
	connect(ActionDeselectAll, &QAction::triggered, [=] { ListFields->SetCheckedItems(false); });
	ToolBar->addAction(ActionDeselectAll);

	ListFields = new ISListWidget(TabFields);
	ListFields->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ListFields->setDragEnabled(true);
	ListFields->setAcceptDrops(true);
	ListFields->setDropIndicatorShown(true);
	ListFields->setDefaultDropAction(Qt::MoveAction);
	LayoutFields->addWidget(ListFields);

	for (unsigned int i = 0, c = TcpModel->columnCount(); i < c; ++i)
	{
		ISModelField ModelField = TcpModel->GetField(i);
		if (!ModelField.IsSystem)
		{
			CreateFieldItem(TcpModel->GetField(i));
		}
	}
	connect(ListFields, &QListWidget::itemChanged, this, &ISExportDialog::FieldsPositionChanged);
	connect(ListFields, &QListWidget::itemDoubleClicked, this, &ISExportDialog::ItemDoubleClicked);
	connect(ListFields->model(), &QAbstractListModel::rowsMoved, this, &ISExportDialog::FieldsPositionChanged);
}
//-----------------------------------------------------------------------------
void ISExportDialog::Select()
{
	if (SelectedFields.empty())
	{
		ISMessageBox::ShowWarning(this, LANG("Export.NotSelectedFields"));
		return;
	}
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISExportDialog::CreateFieldItem(const ISModelField &ModelField)
{
	QListWidgetItem *FieldItem = new QListWidgetItem(ListFields);
	FieldItem->setText(ModelField.LocalName);
	FieldItem->setData(Qt::UserRole, ModelField.Index);
	FieldItem->setCheckState(Qt::Checked);
	FieldItem->setSizeHint(QSize(FieldItem->sizeHint().width(), 30));
	SelectedFields.emplace_back((unsigned int)ModelField.Index);
}
//-----------------------------------------------------------------------------
void ISExportDialog::FieldsPositionChanged()
{
	SelectedFields.clear();
	for (int i = 0; i < ListFields->count(); ++i)
	{
		QListWidgetItem *ListWidgetItem = ListFields->item(i);
		if (!ListWidgetItem->text().isEmpty())
		{
			if (ListWidgetItem->checkState() == Qt::Checked)
			{
				SelectedFields.emplace_back(ListWidgetItem->data(Qt::UserRole).toUInt());
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISExportDialog::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ListWidgetItem->checkState() == Qt::Checked ?
		ListWidgetItem->setCheckState(Qt::Unchecked) :
		ListWidgetItem->setCheckState(Qt::Checked);
}
//-----------------------------------------------------------------------------
void ISExportDialog::EnterClicked()
{
	Select();
}
//-----------------------------------------------------------------------------
void ISExportDialog::TypeChanged(const QVariant &Value)
{
	SelectedType = static_cast<ISNamespace::ExportType>(Value.toInt());
	ButtonDialog->SetApplyEnabled(Value.toInt() != ISNamespace::ET_Unknown);
}
//-----------------------------------------------------------------------------
