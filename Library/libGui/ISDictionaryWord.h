//-----------------------------------------------------------------------------
#pragma once
#ifndef _ISDICTIONARYWORD_H_INCLUDED
#define _ISDICTIONARYWORD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISDictionaryWord
{
public:
	static ISDictionaryWord& Instance();

	QString GetErrorString() const;
	bool Initialize();

private:
	ISDictionaryWord();
	~ISDictionaryWord();
	ISDictionaryWord(ISDictionaryWord const &) {};
	ISDictionaryWord& operator=(ISDictionaryWord const&) { return *this; };

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
#endif
