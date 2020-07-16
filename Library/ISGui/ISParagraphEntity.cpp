#include "ISParagraphEntity.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISAssert.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPHS = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname, prhs_default "
											 "FROM _paragraphs "
											 "WHERE NOT prhs_isdeleted "
											 "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
ISParagraphEntity::ISParagraphEntity()
	: ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISParagraphEntity::~ISParagraphEntity()
{
	while (!Paragraphs.empty())
	{
		delete ISAlgorithm::VectorTakeBack(Paragraphs);
	}
}
//-----------------------------------------------------------------------------
ISParagraphEntity& ISParagraphEntity::Instance()
{
	static ISParagraphEntity ParagraphEntity;
	return ParagraphEntity;
}
//-----------------------------------------------------------------------------
QString ISParagraphEntity::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISParagraphEntity::Initialize()
{
	QString ParagraphView = SETTING_STRING(CONST_UID_SETTING_VIEW_PARAGRAPHVIEW);
	QStringList EnabledParagraphs = ParagraphView.split(SYMBOL_COMMA);

	ISQuery qSelect(QS_PARAGRAPHS);
	bool Result = qSelect.Execute();
	if (Result)
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
				Paragraphs.push_back(MetaParagraph);
				if (Default || SETTING_STRING(CONST_UID_SETTING_VIEW_STARTEDPARAGRAPH) == UID)
				{
					DefaultParagraph = UID;
				}
			}
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
ISUuid ISParagraphEntity::GetDefaultParagraph() const
{
	return DefaultParagraph;
}
//-----------------------------------------------------------------------------
ISMetaParagraph* ISParagraphEntity::GetParagraph(const QString &ParagraphUID)
{
	for (ISMetaParagraph *MetaParagraph : Paragraphs)
	{
		if (MetaParagraph->UID == ParagraphUID)
		{
			return MetaParagraph;
		}
	}
	IS_ASSERT(false, QString("Not found meta paragraph from uid: %1").arg(ParagraphUID));
	return nullptr;
}
//-----------------------------------------------------------------------------
std::vector<ISMetaParagraph*> ISParagraphEntity::GetParagraphs()
{
	return Paragraphs;
}
//-----------------------------------------------------------------------------
