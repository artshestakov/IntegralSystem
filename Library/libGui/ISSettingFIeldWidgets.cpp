#include "ISSettingFIeldWidgets.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISSystem.h"
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

	QWidget *WidgetPanel = new QWidget(this);
	WidgetPanel->setLayout(new QVBoxLayout());
	AddWidgetEdit(WidgetPanel, this);

	ISQuery qSelect(QS_PARAGPAPH);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid ParagraphUID = qSelect.ReadColumn("prhs_uid");
			QString ParagpraphName = qSelect.ReadColumn("prhs_localname").toString();

			QCheckBox *CheckBox = new QCheckBox(this);
			CheckBox->setText(ParagpraphName);
			CheckBox->setCursor(CURSOR_POINTING_HAND);
			CheckBox->setObjectName(ParagraphUID);
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
	if (value.isValid())
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
			StringList.append(AbstractButton->objectName());
		}
	}

	if (StringList.count() == ButtonGroup->buttons().count())
	{
		return "All";
	}
	else if (StringList.count())
	{
		QString Result;
		for (const QString &String : StringList)
		{
			Result.append(String + SYMBOL_COMMA);
		}

		ISSystem::RemoveLastSymbolFromString(Result);
		return Result;
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISCheckViewParagraph::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISCheckViewParagraph::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
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
ISComboIncomingFormPosition::ISComboIncomingFormPosition(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.IncomingForm.Position.LeftUp"), "LeftUp");
	AddItem(LANG("Settings.IncomingForm.Position.LeftBottom"), "LeftBottom");
	AddItem(LANG("Settings.IncomingForm.Position.RightUp"), "RightUp");
	AddItem(LANG("Settings.IncomingForm.Position.RightBottom"), "RightBottom");
	AddItem(LANG("Settings.IncomingForm.Position.Center"), "Center");
}
//-----------------------------------------------------------------------------
ISComboIncomingFormPosition::~ISComboIncomingFormPosition()
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
ISComboTableDateFormat::ISComboTableDateFormat(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.DateFormat.1"), FORMAT_DATE_V1);
	AddItem(LANG("Settings.Table.DateFormat.2"), FORMAT_DATE_V2);
	AddItem(LANG("Settings.Table.DateFormat.3"), FORMAT_DATE_V3);
	AddItem(LANG("Settings.Table.DateFormat.4"), FORMAT_DATE_V4);
}
//-----------------------------------------------------------------------------
ISComboTableDateFormat::~ISComboTableDateFormat()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboTableTimeFormat::ISComboTableTimeFormat(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.TimeFormat.1"), FORMAT_TIME_V1);
	AddItem(LANG("Settings.Table.TimeFormat.2"), FORMAT_TIME_V2);
	AddItem(LANG("Settings.Table.TimeFormat.3"), FORMAT_TIME_V3);
	AddItem(LANG("Settings.Table.TimeFormat.4"), FORMAT_TIME_V5);
}
//-----------------------------------------------------------------------------
ISComboTableTimeFormat::~ISComboTableTimeFormat()
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboTableSelectionBehavior::ISComboTableSelectionBehavior(QWidget *parent) : ISComboSettingEdit(parent)
{
	AddItem(LANG("Settings.Table.SelectionBehavior.SelectItems"), "SelectItems");
	AddItem(LANG("Settings.Table.SelectionBehavior.SelectRows"), "SelectRows");
	AddItem(LANG("Settings.Table.SelectionBehavior.SelectColumns"), "SelectColumns");
}
//-----------------------------------------------------------------------------
ISComboTableSelectionBehavior::~ISComboTableSelectionBehavior()
{

}
//-----------------------------------------------------------------------------
