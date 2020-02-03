#include "ISCheckViewParagraph.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_PARAFGRAPH = PREPARE_QUERY("SELECT prhs_uid, prhs_localname "
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

	ISQuery qSelect(QS_PARAFGRAPH);
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
