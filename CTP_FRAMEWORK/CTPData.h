/*****************************************************************************
File name: CTPData.h
Description: Defined CTP data set.
Author: jebin
Date: 2014/07/10
History: see git log
*****************************************************************************/

#ifndef CTPDATA_H
#define CTPDATA_H

#include "CTPStruct.h"
#include "CLock.h"

#include "ThostTraderApi\ThostFtdcUserApiStruct.h"

#include <list>
#include <WinSock2.h>
#include <Windows.h>
#include <map>
#include <vector>
#include <set>
#include <utility>
using namespace std;
class CTPData
{

    
public:
    //�Ƿ��ѻ�ȡ��Լ
     bool bIsGetInst;
     bool bIsTdConnected;
     bool bIsMdConnected;

    
    //�Ƿ������¼�
     HANDLE h_connected;
     HANDLE h_hasInst;
    
    
    //���ս������к�Լ������Ϣ
     CRITICAL_SECTION v_csInstInfo;
     vector<CThostFtdcInstrumentField> v_instInfo;
     string strAllIns;
    
    //��ǰ��Լ������Ϣ
     CRITICAL_SECTION m_csMarketData;
     map<string, CThostFtdcDepthMarketDataField> m_marketData;
    
   //���б���
     CRITICAL_SECTION m_csOrders;
     map<pair<int, pair<int, string> >, CThostFtdcOrderField> m_orders;
    
    //���гֲ�
     CRITICAL_SECTION m_csPosition;
     map<pair<string, char>, CThostFtdcInvestorPositionField> m_position;
    
    //������Ϣ
     CRITICAL_SECTION v_csErrorInfo;
     vector<string> v_errorInfo;
    
    CTPData()
    {
        bIsGetInst = false;
        bIsTdConnected = false;
        bIsMdConnected = false;
        h_connected = CreateEvent(NULL, FALSE, FALSE, NULL);
        h_hasInst = CreateEvent(NULL, FALSE, FALSE, NULL);
        strAllIns = "";
        v_instInfo.clear();
        m_marketData.clear();
        m_orders.clear();
        m_position.clear();
        v_errorInfo.clear();
        InitializeCriticalSection(&v_csInstInfo);
        InitializeCriticalSection(&m_csMarketData);
        InitializeCriticalSection(&m_csOrders);
        InitializeCriticalSection(&m_csPosition);
        InitializeCriticalSection(&v_csErrorInfo);
    }
    ~CTPData()
    {
        CloseHandle(h_connected);
        CloseHandle(h_hasInst);
        DeleteCriticalSection(&v_csInstInfo);
        DeleteCriticalSection(&m_csMarketData);
        DeleteCriticalSection(&m_csOrders);
        DeleteCriticalSection(&m_csPosition);
        DeleteCriticalSection(&v_csErrorInfo);
    }
    //��ȡ������Ϣ
    CThostFtdcDepthMarketDataField &GetMarketData(string ins)
    {
        CLock cl(&m_csMarketData);
        return m_marketData[ins];
    }
    //��ȡ��Ч����Ϣ
    map<pair<int, pair<int, string> >, CThostFtdcOrderField> &GetOrderInfo()
    {
        CLock cl(&m_csOrders);
        return m_orders;
    }
    //��ȡ�ֲ���Ϣ
    map<pair<string, char>, CThostFtdcInvestorPositionField> &GetPosition()
    {
        CLock cl(&m_csPosition);
        return m_position;
    }
    //��ȡ���ս��׺�Լ��Ϣ
    vector<CThostFtdcInstrumentField> &GetInstInfo()
    {
        CLock cl(&v_csInstInfo);
        return v_instInfo;
    }
    
   vector<string> &GetErrorInfo()
   {
       CLock cl(&v_csErrorInfo);
       return v_errorInfo;
   }

    
};

#endif