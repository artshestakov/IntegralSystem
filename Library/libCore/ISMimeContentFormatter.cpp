#include "ISMimeContentFormatter.h"
//-----------------------------------------------------------------------------
ISMimeContentFormatter::ISMimeContentFormatter(int max_length) : max_length(max_length)
{

}
//-----------------------------------------------------------------------------
QString ISMimeContentFormatter::format(const QString &content, bool quotedPrintable) const 
{
    QString out;

    int chars = 0;
    for (int i = 0; i < content.length() ; ++i) {
        chars++;
        if (!quotedPrintable) {
            if (chars > max_length) {
                    out.append("\r\n");
                    chars = 1;
            }
        }
        else {
            if (content[i] == '\n') {       // new line
                out.append(content[i]);
                chars = 0;
                continue;
            }

            if ((chars > max_length - 1)
                || ((content[i] == '=') && (chars > max_length - 3) )) {
                out.append('=');
                out.append("\r\n");
                chars = 1;
            }

        }
        out.append(content[i]);
    }

    return out;
}
//-----------------------------------------------------------------------------
void ISMimeContentFormatter::setMaxLength(int l) 
{
    max_length = l;
}
//-----------------------------------------------------------------------------
int ISMimeContentFormatter::getMaxLength() const 
{
    return max_length;
}
//-----------------------------------------------------------------------------
