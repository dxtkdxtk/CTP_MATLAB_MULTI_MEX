/*****************************************************************************
File name: Connection.cpp
Description: Defined connection struct for operate CTP
Author: jebin
Date: 2014/07/10
History: see git log
*****************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include "CTPMsgQueue.h"
#include "MdUserApi.h"
#include "TraderApi.h"

using namespace std;

        
//定义连接结构
class Connection
{
    
public:
    Connection()
    {
        md = new MdUserApi();
        td = new TraderApi();
    }
   
    ~Connection()
    {
        msgQueue->StopThread();
        msgQueue->Clear();
        td->Disconnect();
        md->Disconnect();
        
        delete td;
        delete md;

    }
    
    void registerMsgQueue(CTPMsgQueue *queue)
    {
        msgQueue = queue;
        md->RegisterMsgQueue(msgQueue);
        td->RegisterMsgQueue(msgQueue);
        msgQueue->StartThread();
    }
    //读取配置信息
    void readInifile(const char* file, const char *servername)
    {
        char tmp[105];
        GetPrivateProfileStringA(servername, "path", "", tmp, sizeof(tmp), file);
        streamPath = tmp;
        GetPrivateProfileStringA(servername, "mdserver", "", tmp, sizeof(tmp), file);
        mdServer = tmp;
        GetPrivateProfileStringA(servername, "tdserver", "", tmp, sizeof(tmp), file);
        tdServer = tmp;
        GetPrivateProfileStringA(servername, "brokerid", "", tmp, sizeof(tmp), file);
        brokerId = tmp;
        GetPrivateProfileStringA(servername, "investorid", "", tmp, sizeof(tmp), file);
        investorId = tmp;
        GetPrivateProfileStringA(servername, "password", "", tmp, sizeof(tmp), file);
        password = tmp;
        GetPrivateProfileStringA(servername, "instrument", "", tmp, sizeof(tmp), file);
        instrumentId = tmp;
        
    }
    
    CTPMsgQueue *msgQueue;//回报消息队列
//     FunctionCallBackSet *callbackSet;//实时回调信息
    MdUserApi *md;//行情连接口
    TraderApi *td;//交易连接口
    string streamPath;//数据流路径
    string mdServer;//行情服务器
    string tdServer;//交易服务器
    string brokerId;//期商ID
    string investorId;//投资者ID
    string password;//投资者密码
    string instrumentId;//合约名
    

};

#endif

