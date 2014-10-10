#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Connection.h"
#include "CTPMsgQueue.h"

class Account
{
public:
    Account()
    {
        msgQueue = new CTPMsgQueue();
        con = new Connection();
        con->RegisterMsgQueue(msgQueue);
        msgQueue->RegisterConnection(con);
    }
    ~Account()
    {
        delete con;
        delete msgQueue;
        con = NULL;
        msgQueue = NULL;
    }
private:
    CTPMsgQueue *msgQueue;
    Connection *con;
};

#endif