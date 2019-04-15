#include "StdAfx.h"
#include "ISMimeFile.h"
//-----------------------------------------------------------------------------
ISMimeFile::ISMimeFile(QFile *file)
{
    this->file = file;
    this->cType = "application/octet-stream";
    this->cName = QFileInfo(*file).fileName();
    this->cEncoding = Base64;
}
//-----------------------------------------------------------------------------
ISMimeFile::ISMimeFile(const QByteArray& stream, const QString& fileName)
{
    this->cEncoding = Base64;
    this->cType = "application/octet-stream";
    this->file = 0;
    this->cName = fileName;
    this->content = stream;
}
//-----------------------------------------------------------------------------
ISMimeFile::~ISMimeFile()
{
	if (file)
	{
		delete file;
	}
}
//-----------------------------------------------------------------------------
void ISMimeFile::prepare()
{
  if (this->file)
  {
    file->open(QIODevice::ReadOnly);
    this->content = file->readAll();
    file->close();
  }

  ISMimePart::prepare();
}
//-----------------------------------------------------------------------------
