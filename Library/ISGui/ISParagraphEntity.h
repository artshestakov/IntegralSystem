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
	bool Initialize();
	ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //Получить параграф
	std::vector<ISMetaParagraph*> GetParagraphs(); //Получить список параграфов
	ISUuid GetStartedParagraph() const; //Получить стартовый параграф

private:
	ISParagraphEntity();
	~ISParagraphEntity();
	ISParagraphEntity(ISParagraphEntity const &) {};
	ISParagraphEntity& operator=(ISParagraphEntity const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISMetaParagraph*> Paragraphs;
	ISUuid StartedParagraph;
};
//-----------------------------------------------------------------------------
