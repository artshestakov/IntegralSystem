#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISParagraphEntity
{
public:
    static ISParagraphEntity& Instance();

    void Initialize(const QVariantList &VariantList);
    ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //Получить параграф
    std::vector<ISMetaParagraph*> GetParagraphs(); //Получить список параграфов

private:
    ISParagraphEntity();
    ~ISParagraphEntity();
    ISParagraphEntity(const ISParagraphEntity&) = delete;
    ISParagraphEntity(ISParagraphEntity&&) = delete;
    ISParagraphEntity& operator=(const ISParagraphEntity&) = delete;
    ISParagraphEntity& operator=(ISParagraphEntity&&) = delete;

private:
    std::vector<ISMetaParagraph*> Paragraphs;
};
//-----------------------------------------------------------------------------
