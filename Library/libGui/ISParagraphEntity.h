#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMetaParagraph.h"
//-----------------------------------------------------------------------------
class ISParagraphEntity
{
public:
	ISParagraphEntity(const ISParagraphEntity &) = delete;
	ISParagraphEntity(ISParagraphEntity &&) = delete;
	ISParagraphEntity &operator=(const ISParagraphEntity &) = delete;
	ISParagraphEntity &operator=(ISParagraphEntity &&) = delete;
	~ISParagraphEntity();

	static ISParagraphEntity& GetInstance();
	
	ISUuid GetDefaultParagraph() const;
	ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //Получить параграф
	QVector<ISMetaParagraph*> GetParagraphs(); //Получить список параграфов

private:
	ISParagraphEntity();

	QVector<ISMetaParagraph*> Paragraphs;
	ISUuid DefaultParagraph;
};
//-----------------------------------------------------------------------------
