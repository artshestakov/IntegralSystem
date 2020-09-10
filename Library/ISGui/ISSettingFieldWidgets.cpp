#include "ISSettingFIeldWidgets.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISDefinesGui.h"
//-----------------------------------------------------------------------------
static QString QS_PARAGPAPH = PREPARE_QUERY("SELECT prhs_localname, prhs_uid "
											 "FROM _paragraphs "
											 "WHERE NOT prhs_isdeleted "
											 "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
ISCheckViewParagraph::ISCheckViewParagraph(QWidget *parent) : ISFieldEditBase(parent)
{
	ButtonGroup = new QButtonGroup(this);
	ButtonGroup->setExclusive(false);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ISCheckViewParagraph::ValueChanged);

	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetPanel = new QWidget(this);
	WidgetPanel->setLayout(Layout);
	AddWidgetEdit(WidgetPanel, this);

	ISQuery qSelect(QS_PARAGPAPH);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QCheckBox *CheckBox = new QCheckBox(this);
			CheckBox->setText(qSelect.ReadColumn("prhs_localname").toString());
			CheckBox->setCursor(CURSOR_POINTING_HAND);
			CheckBox->setObjectName(ISUuid(qSelect.ReadColumn("prhs_uid")));
			WidgetPanel->layout()->addWidget(CheckBox);
			ButtonGroup->addButton(CheckBox);
		}
	}
}
//-----------------------------------------------------------------------------
ISCheckViewParagraph::~ISCheckViewParagraph()
{

}
//-----------------------------------------------------------------------------
void ISCheckViewParagraph::SetValue(const QVariant &value)
{
	if (value.isValid() && !value.isNull())
	{
		if (value.toString() == "All")
		{
			for (QAbstractButton *AbstractButton : ButtonGroup->buttons())
			{
				AbstractButton->setChecked(true);
			}
		}
		else
		{
			QStringList StringList = value.toString().split(SYMBOL_COMMA);
			for (const QString &String : StringList)
			{
				QCheckBox *CheckBox = findChild<QCheckBox*>(String);
				if (CheckBox)
				{
					CheckBox->setChecked(true);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
QVariant ISCheckViewParagraph::GetValue() const
{
	QStringList StringList;
	for (QAbstractButton *AbstractButton : ButtonGroup->buttons())
	{
		if (AbstractButton->isChecked())
		{
			StringList.push_back(AbstractButton->objectName());
		}
	}

	//Если выбраны все параграфы
	if (StringList.size() == ButtonGroup->buttons().count())
	{
		return "All";
	}
	
	//Выбраны не все параграфы
	if (!StringList.empty())
	{
		return StringList.join(SYMBOL_COMMA);
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
void ISCheckViewParagraph::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboSettingEdit::ISComboSettingEdit(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);
}
//-----------------------------------------------------------------------------
ISComboSettingEdit::~ISComboSettingEdit()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboCalendarNoteType::ISComboCalendarNoteType(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.CalendarEvents.NoteView.ToolTip"), "ToolTip");
	AddItem(LANG("Settings.CalendarEvents.NoteView.List"), "List");
}
//-----------------------------------------------------------------------------
ISComboCalendarNoteType::~ISComboCalendarNoteType()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboStartedParagraph::ISComboStartedParagraph(QWidget *parent) : ISComboSettingEdit(parent)
{
	ISQuery qSelect(QS_PARAGPAPH);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			AddItem(qSelect.ReadColumn("prhs_localname").toString(), ISUuid(qSelect.ReadColumn("prhs_uid")));
		}
	}
}
//-----------------------------------------------------------------------------
ISComboStartedParagraph::~ISComboStartedParagraph()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboTabBarSelectionBehavior::ISComboTabBarSelectionBehavior(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.TabBar.SelectionBehavior.SelectLeftTab"), "SelectLeftTab");
	AddItem(LANG("Settings.TabBar.SelectionBehavior.SelectRightTab"), "SelectRightTab");
	AddItem(LANG("Settings.TabBar.SelectionBehavior.SelectPreviousTab"), "SelectPreviousTab");
}
//-----------------------------------------------------------------------------
ISComboTabBarSelectionBehavior::~ISComboTabBarSelectionBehavior()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboTableDoubleClickEvent::ISComboTableDoubleClickEvent(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.DoubleClickEvent.Window"), "Window");
	AddItem(LANG("Settings.Table.DoubleClickEvent.Tab"), "Tab");
}
//-----------------------------------------------------------------------------
ISComboTableDoubleClickEvent::~ISComboTableDoubleClickEvent()
{

}
//-----------------------------------------------------------------------------
