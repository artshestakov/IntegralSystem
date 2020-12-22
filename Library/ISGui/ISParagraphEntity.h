#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISParagraphEntity
{
public:
	static ISParagraphEntity& Instance();
	
	QString GetErrorString() const;
	void Initialize(const QVariantList &VariantList);
	ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //Получить параграф
	std::vector<ISMetaParagraph*> GetParagraphs(); //Получить список параграфов

private:
	ISParagraphEntity();
	~ISParagraphEntity();
	ISParagraphEntity(ISParagraphEntity const &) {};
	ISParagraphEntity& operator=(ISParagraphEntity const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISMetaParagraph*> Paragraphs;
};
//-----------------------------------------------------------------------------
