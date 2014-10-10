/*****************************************************************************
File name: CTPMsgQueue.h
Description: Defined a queue to process CTP callback message.
Author: jebin
Date: 2014/07/10
History: see git log
*****************************************************************************/

#ifndef CTP_QUEUE_H
#define CTP_QUEUE_H

#include "CTPStruct.h"
#include "LockFreeQ.h"
#include "Connection.h"
#include "CTPData.h"

#include "mex.h"

#include <iostream>

using namespace std;

class Connection;
class CTPMsgQueue
{
    //响应队列中可能出现的消息类型（按字母排序）
    enum EnumMsgType
    {
        E_fnOnConnect,
        E_fnOnDisconnect,
        E_fnOnErrRtnOrderAction,
        E_fnOnErrRtnOrderInsert,
        E_fnOnRspError,
        E_fnOnRspOrderAction,
        E_fnOnRspOrderInsert,
        E_fnOnRspQryDepthMarketData,
        E_fnOnRspQryInstrument,
        E_fnOnRspQryInstrumentCommissionRate,
        E_fnOnRspQryInstrumentMarginRate,
        E_fnOnRspQryInvestorPosition,
        E_fnOnRspQryInvestorPositionDetail,
        E_fnOnRspQryOrder,
        E_fnOnRspQryTrade,
        E_fnOnRspQryTradingAccount,
        E_fnOnRtnDepthMarketData,
        E_fnOnRtnInstrumentStatus,
        E_fnOnRtnOrder,
        E_fnOnRtnTrade,
    };

    struct SMsgItem
    {
        EnumMsgType							type;			//消息类型
        void*								pApi;			//指向类对象的指针
        CThostFtdcRspInfoField				RspInfo;		//响应信息
        bool								bIsLast;		//是否最后一个包
        union{
            int								nRequestID;
            ConnectionStatus				Status;
        };
        union{
            CThostFtdcDepthMarketDataField			DepthMarketData;	//E_fnOnRspQryDepthMarketData与E_fnOnRtnDepthMarketData返回的数据格式相同
            CThostFtdcInputOrderField				InputOrder;
            CThostFtdcInputOrderActionField			InputOrderAction;
            CThostFtdcInstrumentField				Instrument;
            CThostFtdcInstrumentCommissionRateField	InstrumentCommissionRate;
            CThostFtdcInstrumentMarginRateField		InstrumentMarginRate;
            CThostFtdcInstrumentStatusField			InstrumentStatus;
            CThostFtdcInvestorPositionField			InvestorPosition;
            CThostFtdcInvestorPositionDetailField	InvestorPositionDetail;
            CThostFtdcOrderField					Order;
            CThostFtdcOrderActionField				OrderAction;
            CThostFtdcRspUserLoginField				RspUserLogin;
            CThostFtdcTradeField					Trade;
            CThostFtdcTradingAccountField			TradingAccount;
        };
    };

public:
    CTPMsgQueue(void)
    {
        m_hThread = NULL;
        m_bRunning = false;
        ctpData = NULL;
        m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    virtual ~CTPMsgQueue(void)
    {
        StopThread();
        Clear();
        delete ctpData;
        ctpData = NULL;
        CloseHandle(m_hEvent);
    }

public:
    //清空队列
    void Clear();

    //可以由外部发起，顺序处理队列触发回调函数
    bool Process();

    //由内部启动线程，内部主动调用Process触发回调
    void StartThread();
    void StopThread();

    // 注册账户连接
    void RegisterConnection(Connection *tCon);
    //响应结果处理后入队列(按字母排序)
    void Input_OnConnect(void* pApi, CThostFtdcRspUserLoginField *pRspUserLogin, ConnectionStatus result);
    void Input_OnDisconnect(void* pApi, CThostFtdcRspInfoField *pRspInfo, ConnectionStatus step);
    void Input_OnErrRtnOrderAction(void* pTraderApi, CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
    void Input_OnErrRtnOrderInsert(void* pTraderApi, CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
    void Input_OnRspError(void* pApi, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspOrderAction(void* pTraderApi, CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspOrderInsert(void* pTraderApi, CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryDepthMarketData(void* pTraderApi, CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryInvestorPosition(void* pTraderApi, CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryInvestorPositionDetail(void* pTraderApi, CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryInstrument(void* pTraderApi, CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryInstrumentCommissionRate(void* pTraderApi, CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryInstrumentMarginRate(void* pTraderApi, CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryOrder(void* pTraderApi, CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryTrade(void* pTraderApi, CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRspQryTradingAccount(void* pTraderApi, CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void Input_OnRtnDepthMarketData(void* pMdApi, CThostFtdcDepthMarketDataField *pDepthMarketData);
    void Input_OnRtnInstrumentStatus(void* pTraderApi, CThostFtdcInstrumentStatusField *pInstrumentStatus);
    void Input_OnRtnOrder(void* pTraderApi, CThostFtdcOrderField *pOrder);
    void Input_OnRtnTrade(void* pTraderApi, CThostFtdcTradeField *pTrade);
private:
    friend DWORD WINAPI ProcessThread(LPVOID lpParam);
    void RunInThread();

    //响应结果直接入队列
    void _Input_MD(SMsgItem* pMsgItem);
    void _Input_TD(SMsgItem* pMsgItem);
    //队列中的消息判断分发
    void _Output_MD(SMsgItem* pMsgItem);
    void _Output_TD(SMsgItem* pMsgItem);

    //响应输出
    void Output_OnConnect(SMsgItem* pItem); 
    void Output_OnDisconnect(SMsgItem* pItem);
    void Output_OnErrRtnOrderAction(SMsgItem* pItem);
    void Output_OnErrRtnOrderInsert(SMsgItem* pItem);
    void Output_OnRspError(SMsgItem* pItem);
    void Output_OnRspOrderInsert(SMsgItem* pItem);
    void Output_OnRspQryInvestorPosition(SMsgItem* pItem);
    void Output_OnRspQryInvestorPositionDetail(SMsgItem* pItem);
    void Output_OnRspQryDepthMarketData(SMsgItem* pItem);
    void Output_OnRspQryInstrument(SMsgItem* pItem);
    void Output_OnRspQryInstrumentCommissionRate(SMsgItem* pItem);
    void Output_OnRspQryInstrumentMarginRate(SMsgItem* pItem);
    void Output_OnRspQryOrder(SMsgItem* pItem);
    void Output_OnRspQryTrade(SMsgItem* pItem);
    void Output_OnRspQryTradingAccount(SMsgItem* pItem);
    void Output_OnRtnDepthMarketData(SMsgItem* pItem);
    void Output_OnRtnInstrumentStatus(SMsgItem* pItem);
    void Output_OnRtnOrder(SMsgItem* pItem);
    void Output_OnRtnTrade(SMsgItem* pItem);

private:
    volatile bool				m_bRunning;
    HANDLE						m_hEvent;
    HANDLE						m_hThread;
    MSQueue<SMsgItem*>			m_queue_MD;			//响应队列
    MSQueue<SMsgItem*>			m_queue_TD;			//响应队列
    CTPData *ctpData; //保存ctp数据空间
    Connection *Con; // 当前队列对应连接
};

#endif
