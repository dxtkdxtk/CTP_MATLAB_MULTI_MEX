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
    //是否已获取合约
     bool bIsGetInst;
     bool bIsTdConnected;
     bool bIsMdConnected;

    
    //是否连接事件
     HANDLE h_connected;
     HANDLE h_hasInst;
    
    
    //当日交易所有合约基本信息
     CRITICAL_SECTION v_csInstInfo;
     vector<CThostFtdcInstrumentField> v_instInfo;
     string strAllIns;
    
    //当前合约行情信息
     CRITICAL_SECTION m_csMarketData;
     map<string, CThostFtdcDepthMarketDataField> m_marketData;
    
   //所有报单
     CRITICAL_SECTION m_csOrders;
     map<pair<int, pair<int, string> >, CThostFtdcOrderField> m_orders;
    
    //所有持仓
     CRITICAL_SECTION m_csPosition;
     map<pair<string, char>, CThostFtdcInvestorPositionField> m_position;
    
    //错误信息
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
    //获取行情信息
    CThostFtdcDepthMarketDataField &GetMarketData(string ins)
    {
        CLock cl(&m_csMarketData);
        return m_marketData[ins];
    }
    //获取有效单信息
    map<pair<int, pair<int, string> >, CThostFtdcOrderField> &GetOrderInfo()
    {
        CLock cl(&m_csOrders);
        return m_orders;
    }
    //获取持仓信息
    map<pair<string, char>, CThostFtdcInvestorPositionField> &GetPosition()
    {
        CLock cl(&m_csPosition);
        return m_position;
    }
    //获取当日交易合约信息
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