#include "ISParagraphEntity.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPHS = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname, prhs_default "
											 "FROM _paragraphs "
											 "WHERE NOT prhs_isdeleted "
											 "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
ISParagraphEntity::ISParagraphEntity()
{
	QString ParagraphView = SETTING_STRING(CONST_UID_SETTING_VIEW_PARAGRAPHVIEW);
	QStringList EnabledParagraphs = ParagraphView.split(SYMBOL_COMMA);

	ISQuery qSelect(QS_PARAGRAPHS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid UID = qSelect.ReadColumn("prhs_uid");
			QString Name = qSelect.ReadColumn("prhs_name").toString();
			QString LocalName = qSelect.ReadColumn("prhs_localname").toString();
			QString ToolTip = qSelect.ReadColumn("prhs_tooltip").toString();
			QString Icon = qSelect.ReadColumn("prhs_icon").toString();
			QString ClassName = qSelect.ReadColumn("prhs_classname").toString();
			bool Default = qSelect.ReadColumn("prhs_default").toBool();

			if (ParagraphView == "All" || EnabledParagraphs.contains(UID))
			{
				ISMetaParagraph *MetaParagraph = new ISMetaParagraph(UID, Name, LocalName, ToolTip, Icon, ClassName, Default);
				Paragraphs.append(MetaParagraph);

				if (Default || SETTING_STRING(CONST_UID_SETTING_VIEW_STARTEDPARAGRAPH) == UID)
				{
					DefaultParagraph = UID;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
ISParagraphEntity::~ISParagraphEntity()
{
	while (!Paragraphs.isEmpty())
	{
		delete Paragraphs.takeLast();
	}
}
//-----------------------------------------------------------------------------
ISParagraphEntity& ISParagraphEntity::GetInstance()
{
	static ISParagraphEntity ParagraphEntity;
	return ParagraphEntity;
}
//-----------------------------------------------------------------------------
ISUuid ISParagraphEntity::GetDefaultParagraph() const
{
	return DefaultParagraph;
}
//-----------------------------------------------------------------------------
ISMetaParagraph* ISParagraphEntity::GetParagraph(const QString &ParagraphUID)
{
	for (int i = 0; i < Paragraphs.count(); ++i)
	{
		if (Paragraphs.at(i)->GetUID() == ParagraphUID)
		{
			return Paragraphs.at(i);
		}
	}

	IS_ASSERT(false, QString("Not found meta paragraph from uid: %1").arg(ParagraphUID));
	return nullptr;
}
//-----------------------------------------------------------------------------
QVector<ISMetaParagraph*> ISParagraphEntity::GetParagraphs()
{
	return Paragraphs;
}
//-----------------------------------------------------------------------------
