#include "CTPMsgQueue.h"

void CTPMsgQueue::Output_OnConnect(SMsgItem* pItem)
{
//         if (m_fnOnConnect)
//             (*m_fnOnConnect)(pItem->pApi, &pItem->RspUserLogin, pItem->Status);
    ConnectionStatus result = pItem->Status;
    if( Con->td ==  pItem->pApi && result == E_confirmed)
    {
        ctpData->bIsTdConnected = true;
        SetEvent(ctpData->h_connected);
    }
    else if(Con->md ==  pItem->pApi && result == E_logined)
    {
        bIsMdConnected = true;
        SetEvent(ctpData->h_connected);
    }
    
}
void CTPMsgQueue::Output_OnDisconnect(SMsgItem* pItem)
{
//         if (m_fnOnDisconnect)
//             (*m_fnOnDisconnect)(pItem->pApi, &pItem->RspInfo, pItem->Status);
    
    CThostFtdcRspInfoField *pRspInfo = &pItem->RspInfo;
    CLock cl(&ctpData->v_csErrorInfo);
    if(pRspInfo->ErrorID)
    {
        ctpData->v_errorInfo.push_back(string("[OnDisconnect]: ") + string(pRspInfo->ErrorMsg));
    }
}
void CTPMsgQueue::Output_OnErrRtnOrderAction(SMsgItem* pItem)
{
//         if (m_fnOnErrRtnOrderAction)
//             (*m_fnOnErrRtnOrderAction)(pItem->pApi, &pItem->OrderAction, &pItem->RspInfo);
    CThostFtdcRspInfoField *pRspInfo = &pItem->RspInfo;
    CLock cl(&ctpData->v_csErrorInfo);
    if(pRspInfo->ErrorID)
    {
        ctpData->v_errorInfo.push_back(string("[OnErrRtnOrderAction]: ") + string(pRspInfo->ErrorMsg));
    }
}
void CTPMsgQueue::Output_OnErrRtnOrderInsert(SMsgItem* pItem)
{
//         if (m_fnOnErrRtnOrderInsert)
//             (*m_fnOnErrRtnOrderInsert)(pItem->pApi, &pItem->InputOrder, &pItem->RspInfo);
    CThostFtdcRspInfoField *pRspInfo = &pItem->RspInfo;
    CLock cl(&ctpData->v_csErrorInfo);
    if(pRspInfo->ErrorID)
    {
        ctpData->v_errorInfo.push_back(string("[OnErrRtnOrderAction]: ") + string(pRspInfo->ErrorMsg));
    }
    
}
void CTPMsgQueue::Output_OnRspError(SMsgItem* pItem)
{
//         if (m_fnOnRspError)
//             (*m_fnOnRspError)(pItem->pApi, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    CThostFtdcRspInfoField *pRspInfo = &pItem->RspInfo;
    CLock cl(&ctpData->v_csErrorInfo);
    if(pRspInfo->ErrorID)
    {
        ctpData->v_errorInfo.push_back(string("[OnRspError]: ") + string(pRspInfo->ErrorMsg));
    }
}
void CTPMsgQueue::Output_OnRspOrderAction(SMsgItem* pItem)
{
//         if (m_fnOnRspOrderAction)
//             (*m_fnOnRspOrderAction)(pItem->pApi, &pItem->InputOrderAction, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
//
    CThostFtdcRspInfoField *pRspInfo = &pItem->RspInfo;
    CLock cl(&ctpData->v_csErrorInfo);
    if(pRspInfo->ErrorID == 0)
    {
        ctpData->v_errorInfo.push_back(string("[OnRspOrderAction]: ") + string("撤单成功!"));
    }
    else
    {
        ctpData->v_errorInfo.push_back(string("[OnRspOrderAction]: ") + string(pRspInfo->ErrorMsg));
    }
}
void CTPMsgQueue::Output_OnRspOrderInsert(SMsgItem* pItem)
{
//         if (m_fnOnRspOrderInsert)
//             (*m_fnOnRspOrderInsert)(pItem->pApi, &pItem->InputOrder, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    CThostFtdcRspInfoField *pRspInfo = &pItem->RspInfo;
    CLock cl(&ctpData->v_csErrorInfo);
    if(pRspInfo->ErrorID == 0)
    {
        ctpData->v_errorInfo.push_back(string("[OnRspOrderInsert]: ") + string("下单成功!"));
    }
    else
    {
        ctpData->v_errorInfo.push_back(string("[OnRspOrderInsert]: ") + string(pRspInfo->ErrorMsg));
    }
}
void CTPMsgQueue::Output_OnRspQryInvestorPosition(SMsgItem* pItem)
{
//         if (m_fnOnRspQryInvestorPosition)
//             (*m_fnOnRspQryInvestorPosition)(pItem->pApi, &pItem->InvestorPosition, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
//
    CLock cl(&ctpData->m_csPosition);
    if(pInvestorPosition->Position == 0)
    {
        ctpData->m_position.erase(make_pair(pItem->InvestorPosition.InstrumentID, pItem->InvestorPosition.PosiDirection));
    }
    else
    {
        ctpData->m_position[make_pair(pItem->InvestorPosition.InstrumentID, pItem->InvestorPosition.PosiDirection)] = pItem->InvestorPosition;
    }
}
void CTPMsgQueue::Output_OnRspQryInvestorPositionDetail(SMsgItem* pItem)
{
//     if (m_fnOnRspQryInvestorPositionDetail)
//         (*m_fnOnRspQryInvestorPositionDetail)(pItem->pApi, &pItem->InvestorPositionDetail, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRspQryDepthMarketData(SMsgItem* pItem)
{
//     if (m_fnOnRspQryDepthMarketData)
//         (*m_fnOnRspQryDepthMarketData)(pItem->pApi, &pItem->DepthMarketData, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRspQryInstrument(SMsgItem* pItem)
{
//     if (m_fnOnRspQryInstrument)
//         (*m_fnOnRspQryInstrument)(pItem->pApi, &pItem->Instrument, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    CLock cl(&ctpData->v_csInstInfo);
    

    ctpData->strAllIns += pItem->Instrument.InstrumentID;
    ctpData->strAllIns += ';';
    ctpData->v_instInfo.push_back(pItem->Instrument);
    if(ctpData->v_instInfo.size() >= 100)
    {
        SetEvent(ctpData->h_hasInst);
        ctpData->bIsGetInst = true;
    }
}
void CTPMsgQueue::Output_OnRspQryInstrumentCommissionRate(SMsgItem* pItem)
{
//     if (m_fnOnRspQryInstrumentCommissionRate)
//         (*m_fnOnRspQryInstrumentCommissionRate)(pItem->pApi, &pItem->InstrumentCommissionRate, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRspQryInstrumentMarginRate(SMsgItem* pItem)
{
//     if (m_fnOnRspQryInstrumentMarginRate)
//         (*m_fnOnRspQryInstrumentMarginRate)(pItem->pApi, &pItem->InstrumentMarginRate, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRspQryOrder(SMsgItem* pItem)
{
//     if (m_fnOnRspQryOrder)
//         (*m_fnOnRspQryOrder)(pItem->pApi, &pItem->Order, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRspQryTrade(SMsgItem* pItem)
{
//     if (m_fnOnRspQryTrade)
//         (*m_fnOnRspQryTrade)(pItem->pApi, &pItem->Trade, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRspQryTradingAccount(SMsgItem* pItem)
{
//     if (m_fnOnRspQryTradingAccount)
//         (*m_fnOnRspQryTradingAccount)(pItem->pApi, &pItem->TradingAccount, &pItem->RspInfo, pItem->nRequestID, pItem->bIsLast);
    
}
void CTPMsgQueue::Output_OnRtnDepthMarketData(SMsgItem* pItem)
{
//         if (m_fnOnRtnDepthMarketData)
//             (*m_fnOnRtnDepthMarketData)(pItem->pApi, &pItem->DepthMarketData);
    CLock cl(&ctpData->m_csMarketData);
//     memcpy(&m_marketData[string(pDepthMarketData->InstrumentID)], pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField));
    ctpData->m_marketData[string(pItem->DepthMarketData.InstrumentID)] = pItem->DepthMarketData;
}
void CTPMsgQueue::Output_OnRtnInstrumentStatus(SMsgItem* pItem)
{
//     if (m_fnOnRtnInstrumentStatus)
//         (*m_fnOnRtnInstrumentStatus)(pItem->pApi, &pItem->InstrumentStatus);
    
}
void CTPMsgQueue::Output_OnRtnOrder(SMsgItem* pItem)
{
//         if (m_fnOnRtnOrder)
//             (*m_fnOnRtnOrder)(pItem->pApi, &pItem->Order);
    
    CThostFtdcOrderField *pOrder = &pItem->Order;
    CLock cl(&ctpData->m_csOrders);
    pair<int, pair<int, string> > ref = make_pair(pOrder->FrontID, make_pair(pOrder->SessionID, pOrder->OrderRef));
    ctpData->m_orders[ref] = *pOrder;
    
}
void CTPMsgQueue::Output_OnRtnTrade(SMsgItem* pItem)
{
//     if (m_fnOnRtnTrade)
//         (*m_fnOnRtnTrade)(pItem->pApi, &pItem->Trade);
    
}