#include "StdAfx.h"
#include "ISMimeInlineFile.h"
//-----------------------------------------------------------------------------
ISMimeInlineFile::ISMimeInlineFile(QFile *f) : ISMimeFile(f)
{

}
//-----------------------------------------------------------------------------
ISMimeInlineFile::~ISMimeInlineFile()
{

}
//-----------------------------------------------------------------------------
void ISMimeInlineFile::prepare()
{       
    this->header += "Content-Disposition: inline\r\n";
    ISMimeFile::prepare();
}
//-----------------------------------------------------------------------------
