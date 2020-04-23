#pragma once
#ifndef _ISDICTIONARYWORD_H_INCLUDED
#define _ISDICTIONARYWORD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDictionaryWord
{
public:
	static ISDictionaryWord& Instance();

	QString GetErrorString() const;
	bool Initialize();
	bool Search(const char *String);
	void AddWord(const QString &Word);

private:
	bool ReadFromLocal(); //Чтение локального словаря
	bool ReadFromDB(); //Чтение пользовательского словаря из БД

private:
	ISDictionaryWord();
	~ISDictionaryWord();
	ISDictionaryWord(ISDictionaryWord const &) {};
	ISDictionaryWord& operator=(ISDictionaryWord const&) { return *this; };

private:
	QString ErrorString;
	char **Array;
	size_t ArraySize;
	ISVectorString ArrayUser;
};
//-----------------------------------------------------------------------------
#endif
