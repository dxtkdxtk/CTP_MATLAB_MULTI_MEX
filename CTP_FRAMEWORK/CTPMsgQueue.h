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
    //��Ӧ�����п��ܳ��ֵ���Ϣ���ͣ�����ĸ����
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
        EnumMsgType							type;			//��Ϣ����
        void*								pApi;			//ָ��������ָ��
        CThostFtdcRspInfoField				RspInfo;		//��Ӧ��Ϣ
        bool								bIsLast;		//�Ƿ����һ����
        union{
            int								nRequestID;
            ConnectionStatus				Status;
        };
        union{
            CThostFtdcDepthMarketDataField			DepthMarketData;	//E_fnOnRspQryDepthMarketData��E_fnOnRtnDepthMarketData���ص����ݸ�ʽ��ͬ
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
    //��ն���
    void Clear();

    //�������ⲿ����˳������д����ص�����
    bool Process();

    //���ڲ������̣߳��ڲ���������Process�����ص�
    void StartThread();
    void StopThread();

    // ע���˻�����
    void RegisterConnection(Connection *tCon);
    //��Ӧ�������������(����ĸ����)
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

    //��Ӧ���ֱ�������
    void _Input_MD(SMsgItem* pMsgItem);
    void _Input_TD(SMsgItem* pMsgItem);
    //�����е���Ϣ�жϷַ�
    void _Output_MD(SMsgItem* pMsgItem);
    void _Output_TD(SMsgItem* pMsgItem);

    //��Ӧ���
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
    MSQueue<SMsgItem*>			m_queue_MD;			//��Ӧ����
    MSQueue<SMsgItem*>			m_queue_TD;			//��Ӧ����
    CTPData *ctpData; //����ctp���ݿռ�
    Connection *Con; // ��ǰ���ж�Ӧ����
};

#endif
