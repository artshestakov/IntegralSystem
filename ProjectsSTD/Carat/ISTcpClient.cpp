#include "ISTcpClient.h"
//-----------------------------------------------------------------------------
ISTcpClient::ISTcpClient()
	: Socket(0),
	Port(0),
	BufferSize(0)
{

}
//-----------------------------------------------------------------------------
ISTcpClient::~ISTcpClient()
{

}
//-----------------------------------------------------------------------------
void ISTcpClient::AddData(const char *Data, int Size)
{
	for (int i = 0; i < Size; ++i)
	{
		Buffer.emplace_back(Data[i]);
	}
}
//-----------------------------------------------------------------------------
