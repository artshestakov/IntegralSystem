#include "ISMimeAttachment.h"
//-----------------------------------------------------------------------------
ISMimeAttachment::ISMimeAttachment(QFile *file) : ISMimeFile(file)
{

}
//-----------------------------------------------------------------------------
ISMimeAttachment::ISMimeAttachment(const QByteArray& stream, const QString& fileName): ISMimeFile(stream, fileName)
{

}
//-----------------------------------------------------------------------------
ISMimeAttachment::~ISMimeAttachment()
{

}
//-----------------------------------------------------------------------------
void ISMimeAttachment::prepare()
{
    header += "Content-disposition: attachment\r\n";
	ISMimeFile::prepare();
}
//-----------------------------------------------------------------------------
