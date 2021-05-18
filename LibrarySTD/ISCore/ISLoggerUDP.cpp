#include "ISLoggerUDP.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLoggerUDP::ISLoggerUDP()
    : ErrorString(STRING_NO_ERROR),
    Flag(ISNamespace::Started::Unknown),
    Socket(0)
{
    CRITICAL_SECTION_INIT(&CS);
}
//-----------------------------------------------------------------------------
ISLoggerUDP::~ISLoggerUDP()
{
    CRITICAL_SECTION_DESTROY(&CS);
}
//-----------------------------------------------------------------------------
ISLoggerUDP& ISLoggerUDP::Instance()
{
    static ISLoggerUDP LoggerUDP;
    return LoggerUDP;
}
//-----------------------------------------------------------------------------
const std::string& ISLoggerUDP::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLoggerUDP::Init()
{
    //Запускаем поток
    std::thread(&ISLoggerUDP::Worker, this).detach();
    
    //Ждём результат запуска
    Flag = ISNamespace::Started::Starting;
    while (true)
    {
        ISSleep(1);
        ISNamespace::Started f = ISNamespace::Started::Unknown;
        CRITICAL_SECTION_LOCK(&CS);
        f = Flag;
        CRITICAL_SECTION_UNLOCK(&CS);

        if (f != ISNamespace::Started::Starting)
        {
            break;
        }
    }
    return Flag == ISNamespace::Started::Started;
}
//-----------------------------------------------------------------------------
#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

void ISLoggerUDP::Worker()
{
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    char buf[BUFLEN];
    WSADATA wsa;

    slen = sizeof(si_other);

    //Initialise winsock
    //printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    //printf("Initialised.\n");

    //Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }
    //printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    //puts("Bind done");

    CRITICAL_SECTION_LOCK(&CS);
    Flag = ISNamespace::Started::Started;
    CRITICAL_SECTION_UNLOCK(&CS);

    //keep listening for data
    //std::thread(&ISLoggerUDP::Test, this).detach();
    while (1)
    {
        //printf("Waiting for data...");
        //fflush(stdout);

        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        if (buf[0] == '.')
        {
            while (true)
            {
                CRITICAL_SECTION_LOCK(&CS);
                while (!Queue.empty())
                {
                    std::string S = Queue.front();
                    Queue.pop();

                    if (sendto(s, S.c_str(), S.size(), 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
                    {
                        printf("sendto() failed with error code : %d", WSAGetLastError());
                        exit(EXIT_FAILURE);
                    }
                }
                CRITICAL_SECTION_UNLOCK(&CS);
            }
        }

        //print details of the client/peer and the data received
        //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        //printf("Data: %s\n", buf);

        //now reply the client with the same data
        /*if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }*/
    }

    closesocket(s);
    //WSACleanup();
}
//-----------------------------------------------------------------------------
void ISLoggerUDP::Add(const std::string &String)
{
    CRITICAL_SECTION_LOCK(&CS);
    /*size_t StringSize = String.size();
    for (size_t i = 0, c = Clients.size(); i < c; ++i)
    {
        int Result = sendto(Socket, String.c_str(), StringSize, 0, (struct sockaddr *)&Clients[i], 0);
        if (Result != (int)StringSize)
        {
            ISAlgorithm::GetLastErrorS();
        }
    }*/
    Queue.push(String);
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
void ISLoggerUDP::Test()
{
    ISSleep(2000);

    struct sockaddr_in si_other;
    int s, slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    WSADATA wsa;

    //Initialise winsock
    //printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    //printf("Initialised.\n");

    //create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //setup address structure
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    //start communication
    while (1)
    {
        //printf("Enter message : ");
        //gets(message);
        message[0] = '.';
        message[1] = '\0';

        //send the message
        if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        int r = 0;
        while (true)
        {
            r = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
            if (r == SOCKET_ERROR)
            {
                printf("recvfrom() failed with error code : %d", WSAGetLastError());
            }
            buf[r] = '\0';
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        puts(buf);
    }

    closesocket(s);
    WSACleanup();
}
//-----------------------------------------------------------------------------
