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
	ISMetaParagraph* GetParagraph(const QString &ParagraphUID); //�������� ��������
	std::vector<ISMetaParagraph*> GetParagraphs(); //�������� ������ ����������

private:
	ISParagraphEntity();
	~ISParagraphEntity();
	ISParagraphEntity(ISParagraphEntity const &) {};
	ISParagraphEntity& operator=(ISParagraphEntity const&) { return *this; };

private:
	std::vector<ISMetaParagraph*> Paragraphs;
};
//-----------------------------------------------------------------------------
