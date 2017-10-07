#include "StdAfx.h"
#include ".\tcpsrvexception.h"

CSrvException::CSrvException(void)
{
}
CSrvException::CSrvException(const char* expDescription,int expCode,long expLine)
{
	strcpy(this->m_strExpDescription,expDescription);
	this->m_iExpCode = expCode;
	this->m_iExpLineNumber = expLine;
}
CSrvException::~CSrvException(void)
{
}
