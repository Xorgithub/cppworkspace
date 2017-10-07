#pragma once
#include "tcpserver.h"

class CWebServer :public CTcpServer
{
public:
	CWebServer(void);
	~CWebServer(void);
protected:
	void SaveReceivedData(SOCKET sock,char* pVoid,long dataLen);
private:
	CRITICAL_SECTION m_struCriSec;
	map<SOCKET,CMemoryBlock*> m_mapWebSocketBuffer;
};
