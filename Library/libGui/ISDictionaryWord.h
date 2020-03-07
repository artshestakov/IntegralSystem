#pragma once
#ifndef _ISDICTIONARYWORD_H_INCLUDED
#define _ISDICTIONARYWORD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDictionaryWord
{
public:
	static ISDictionaryWord& Instance();

	QString GetErrorString() const;
	bool Initialize();
	bool Search(const char *String);
	void AddWord(const QString &Word);

private:
	ISDictionaryWord();
	~ISDictionaryWord();
	ISDictionaryWord(ISDictionaryWord const &) {};
	ISDictionaryWord& operator=(ISDictionaryWord const&) { return *this; };

private:
	QString ErrorString;
	char **Array;
	size_t ArraySize;
	std::vector<const char *> ArrayUser;
};
//-----------------------------------------------------------------------------
#endif
