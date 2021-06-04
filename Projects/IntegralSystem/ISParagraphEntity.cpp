#include "ISParagraphEntity.h"
#include "ISConstants.h"
#include "ISSettings.h"
#include "ISAssert.h"
#include "ISAlgorithmOld.h"
//-----------------------------------------------------------------------------
ISParagraphEntity::ISParagraphEntity()
{

}
//-----------------------------------------------------------------------------
ISParagraphEntity::~ISParagraphEntity()
{
    while (!Paragraphs.empty())
    {
        delete ISAlgorithmOld::VectorTakeBack(Paragraphs);
    }
}
//-----------------------------------------------------------------------------
ISParagraphEntity& ISParagraphEntity::Instance()
{
    static ISParagraphEntity ParagraphEntity;
    return ParagraphEntity;
}
//-----------------------------------------------------------------------------
void ISParagraphEntity::Initialize(const QVariantList &VariantList)
{
    for (const QVariant &Paragraph : VariantList)
    {
        QVariantMap ParagraphMap = Paragraph.toMap();
        Paragraphs.emplace_back(new ISMetaParagraph
        {
            ParagraphMap["UID"].toString(),
            ParagraphMap["Name"].toString(),
            ParagraphMap["Local"].toString(),
            ParagraphMap["ToolTip"].toString(),
            ParagraphMap["Icon"].toString(),
            ParagraphMap["Class"].toString()
        });
    }
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
