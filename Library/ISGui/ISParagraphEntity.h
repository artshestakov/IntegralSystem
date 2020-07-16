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
	ISUuid GetDefaultParagraph() const;
	ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //�������� ��������
	std::vector<ISMetaParagraph*> GetParagraphs(); //�������� ������ ����������

private:
	ISParagraphEntity();
	~ISParagraphEntity();
	ISParagraphEntity(ISParagraphEntity const &) {};
	ISParagraphEntity& operator=(ISParagraphEntity const&) { return *this; };

private:
	QString ErrorString;
	std::vector<ISMetaParagraph*> Paragraphs;
	ISUuid DefaultParagraph;
};
//-----------------------------------------------------------------------------
