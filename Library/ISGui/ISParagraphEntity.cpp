#include "ISParagraphEntity.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSettings.h"
#include "ISAssert.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPHS = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
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
void ISParagraphEntity::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &Paragraph : VariantList)
	{
		QVariantMap ParagraphMap = Paragraph.toMap();
		Paragraphs.emplace_back(new ISMetaParagraph
		{
			ParagraphMap["UID"],
			ParagraphMap["Name"].toString(),
			ParagraphMap["Local"].toString(),
			ParagraphMap["ToolTip"].toString(),
			ParagraphMap["Icon"].toString(),
			ParagraphMap["Class"].toString()
		});
	}
}
//-----------------------------------------------------------------------------
bool ISParagraphEntity::Initialize()
{
	ISQuery qSelect(QS_PARAGRAPHS);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			Paragraphs.emplace_back(new ISMetaParagraph
			{
				qSelect.ReadColumn("prhs_uid"),
				qSelect.ReadColumn("prhs_name").toString(),
				qSelect.ReadColumn("prhs_localname").toString(),
				qSelect.ReadColumn("prhs_tooltip").toString(),
				qSelect.ReadColumn("prhs_icon").toString(),
				qSelect.ReadColumn("prhs_classname").toString()
			});
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
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
