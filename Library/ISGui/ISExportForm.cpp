#include "ISExportForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISExportForm::ISExportForm(ISTcpModel *TcpModel)
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
	connect(ComboBoxType, &ISComboEdit::ValueChange, this, &ISExportForm::TypeChanged);
	GetMainLayout()->addWidget(ComboBoxType);

	TabWidget = new QTabWidget(this);
	GetMainLayout()->addWidget(TabWidget);

	CreateTabSettings();
	CreateTabFields(TcpModel);

	ButtonDialog = new ISButtonDialog(this, LANG("Export"));
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISExportForm::Select);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISExportForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
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
ISVectorUInt ISExportForm::GetSelectedFields() const
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
void ISExportForm::CreateTabFields(ISTcpModel *TcpModel)
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

	for (size_t i = 0, c = TcpModel->columnCount(); i < c; ++i)
	{
		ISModelField ModelField = TcpModel->GetField(i);
		if (!ModelField.IsSystem)
		{
			CreateFieldItem(TcpModel->GetField(i));
		}
	}
	connect(ListFields, &QListWidget::itemChanged, this, &ISExportForm::FieldsPositionChanged);
	connect(ListFields, &QListWidget::itemDoubleClicked, this, &ISExportForm::ItemDoubleClicked);
	connect(ListFields->model(), &QAbstractListModel::rowsMoved, this, &ISExportForm::FieldsPositionChanged);
}
//-----------------------------------------------------------------------------
void ISExportForm::Select()
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
void ISExportForm::CreateFieldItem(const ISModelField &ModelField)
{
	QListWidgetItem *FieldItem = new QListWidgetItem(ListFields);
	FieldItem->setText(ModelField.LocalName);
	FieldItem->setData(Qt::UserRole, ModelField.Index);
	FieldItem->setCheckState(Qt::Checked);
	FieldItem->setSizeHint(QSize(FieldItem->sizeHint().width(), 30));
	SelectedFields.emplace_back(ModelField.Index);
}
//-----------------------------------------------------------------------------
void ISExportForm::FieldsPositionChanged()
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
void ISExportForm::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ListWidgetItem->checkState() == Qt::Checked ?
		ListWidgetItem->setCheckState(Qt::Unchecked) :
		ListWidgetItem->setCheckState(Qt::Checked);
}
//-----------------------------------------------------------------------------
void ISExportForm::EnterClicked()
{
	Select();
}
//-----------------------------------------------------------------------------
void ISExportForm::TypeChanged(const QVariant &Value)
{
	SelectedType = static_cast<ISNamespace::ExportType>(Value.toInt());
	ButtonDialog->SetApplyEnabled(Value.toInt() != ISNamespace::ET_Unknown);
}
//-----------------------------------------------------------------------------
