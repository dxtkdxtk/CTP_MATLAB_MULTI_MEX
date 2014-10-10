/*****************************************************************************
 * File name: TraderMain.cpp
 * Description: CTP for matlab mex. Designed a series of operations to operate the CTP
 * Author: jebin
 * Version: 1.0
 * Date: 2014/07/10
 * History: see git log
 *****************************************************************************/

#include "Connection.h"
#include "mxStructTool.h"

#include "mex.h"
#include "matrix.h"

#include<iostream>
#include<cstring>
#include<string>
#include<set>
#include<utility>

using namespace std;

//�����ܲ���
// Connection *Con;
vector<Connection *> v_con;

void CheckIsConnect(int account)
{
    if((v_con.size() >= account || v_con.size() < 0) || NULL == v_con[account])
        mexErrMsgTxt("δ�����˻���");
}

void CheckIsAlloc(Connection *con)
{
    if(NULL == con)
        mexErrMsgTxt("δ�ɹ���������\n");
}
//mex���������
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
    //Ϊ��֤�ٶȣ�δ��Ӱ�ȫ�ж�
    int choise = (int)mxGetScalar(prhs[0]);
    switch(choise)
    {
        
        //����CTP
        case 1:
        {
            Connection *Con = new Connection();
            CheckIsAlloc(Con);
            Con->readInifile(".\\server.ini", mxArrayToString(prhs[1]));
            Con->td->Connect(Con->streamPath,
                    Con->tdServer,
                    Con->brokerId,
                    Con->investorId,
                    Con->password,
                    THOST_TERT_RESTART, "", "");
            WaitForSingleObject(Con->ctpData->h_connected, 5000);
            if(Con->ctpData->bIsTdConnected == false)
            {
                delete Con;
                Con = NULL;
                mexPrintf("���׶�����ʧ��\n");
            }
            else
            {
                mexPrintf("���׶����ӳɹ�\n");
                Con->md->Connect(Con->streamPath,
                        Con->mdServer,
                        Con->brokerId,
                        Con->investorId,
                        Con->password);
                WaitForSingleObject(Con->ctpData->h_connected, 5000);
                if(Con->ctpData->bIsMdConnected == false)
                {
                    delete Con;
                    Con = NULL;
                    mexPrintf("���������ʧ��\n");
                }
                else
                {
                    mexPrintf("��������ӳɹ�\n");
                    plhs[0] = mxCreateDoubleScalar(v_con.size());
                    plhs[1] = mxCreateDoubleScalar(Con->td->m_RspUserLogin.FrontID);
                    plhs[2] = mxCreateDoubleScalar(Con->td->m_RspUserLogin.SessionID);
                    Con->td->ReqQryInstrument("");
                    mexPrintf("��ȡ��Լ�ɹ�\n");
                    v_con.push_back(Con);
                }
            }
            break;
        }
        
        //�Ͽ�CTP
        case 2:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            delete v_con[account];
            v_con[account] = NULL;
            mexPrintf("�Ͽ��˻�%d�ɹ�\n", account);
            break;
        }
        
        //��������
        case 3:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            string inst = mxArrayToString(prhs[2]);
            if(inst.size() == 0)
            {
                if(v_con[account]->ctpData->bIsGetInst)
                {
                    v_con[account]->md->Subscribe(v_con[account]->ctpData->strAllIns);
                    mexPrintf("�������\n");
                }
                else
                    mexWarnMsgTxt("δ��ѯ���к�Լ������ʹ�ö���ȫ����Լ");
            }
            else
            {
                v_con[account]->md->Subscribe(inst);
                mexPrintf("�������\n");
            }
            break;
        }
        
//         ��ѯ��Լ��Ϣ
        case 4:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            plhs[0] = GetInstInfo(v_con[account]->ctpData->GetInstInfo());
            break;
        }
        
//         ��ȡ��Լ��ǰ��������������Ϣ
        case 5:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            string inst = mxArrayToString(prhs[2]);
            plhs[0] = GetMarketData(v_con[account]->ctpData->GetMarketData(inst));
            break;
        }
        
        //�µ�����
        case 6:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            map<pair<int, pair<int, string> >, CThostFtdcOrderField> &orders = v_con[account]->ctpData->m_orders;
            string inst = mxArrayToString(prhs[2]);
            string direction = mxArrayToString(prhs[3]);
            string offsetFlag = mxArrayToString(prhs[4]);
            double volume = mxGetScalar(prhs[5]);
            double price = mxGetScalar(prhs[6]);
            int ref = v_con[account]->td->ReqOrderInsert(inst, direction[0], offsetFlag.c_str(), "1", (int)volume, price,
                    THOST_FTDC_OPT_LimitPrice, THOST_FTDC_TC_GFD, THOST_FTDC_CC_Immediately,
                    0, THOST_FTDC_VC_AV);
            char buf[105];
            itoa (ref,buf,10);
            pair<int, pair<int, string> > order =
                    make_pair(v_con[account]->td->m_RspUserLogin.FrontID, make_pair(v_con[account]->td->m_RspUserLogin.SessionID, string(buf)));
            int timeout = 0;
            while(orders.find(order) == orders.end())
            {
                ++timeout;
                Sleep(1);
                if(timeout > 3000)
                    break;
            }
            if(timeout > 3000)
                plhs[0] = mxCreateDoubleScalar(-1);
            else
                plhs[0] = mxCreateDoubleScalar(ref);
            break;
        }
        
        //��ȡ���б�����Ϣ
        case 7:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            map<pair<int, pair<int, string> >, CThostFtdcOrderField> &orders = v_con[account]->ctpData->m_orders;
            //��ѯ���б���
            if(nrhs == 2)
            {
                pair<int, pair<int, string> > order = make_pair(0, make_pair(0, ""));
                plhs[0] = GetOrderData(v_con[account]->ctpData->GetOrderInfo(), order);
            }
            //��ѯ��ǰ���ӱ���
            else if(nrhs == 3)
            {
                string ref = mxArrayToString(prhs[2]);
                pair<int, pair<int, string> > order =
                        make_pair(v_con[account]->td->m_RspUserLogin.FrontID, make_pair(v_con[account]->td->m_RspUserLogin.SessionID, ref));
                if(orders.find(order) != orders.end())
                    plhs[0] = GetOrderData(v_con[account]->ctpData->GetOrderInfo(), order);
                else
                {
                    plhs[0] = mxCreateDoubleScalar(0);
                    mexWarnMsgTxt("δ���ڴ˱���\n");
                }
            }
            //��ѯ����������ָ������
            else if(nrhs == 5)
            {
                int frontid = (int)mxGetScalar(prhs[2]);
                int session = (int)mxGetScalar(prhs[3]);
                string ref = mxArrayToString(prhs[4]);
                pair<int, pair<int, string> > order =
                        make_pair(frontid, make_pair(session, ref));
                if(orders.find(order) != orders.end())
                    plhs[0] = GetOrderData(v_con[account]->ctpData->GetOrderInfo(), order);
                else
                {
                    plhs[0] = mxCreateDoubleScalar(0);
                    mexWarnMsgTxt("δ���ڴ˱���\n");
                }
            }
            break;
        }
        
        //����
        case 8:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            map<pair<int, pair<int, string> >, CThostFtdcOrderField> &orders = v_con[account]->ctpData->m_orders;
            if(nrhs == 3)
            {
                if( !mxIsChar(prhs[2]) )
                {
                    CThostFtdcOrderField order;
                    MxToOrder(order, prhs[1]);
                    v_con[account]->td->ReqOrderAction(&order);
                }
                else
                {
                    string ref = mxArrayToString(prhs[2]);
                    pair<int, pair<int, string> > order =
                            make_pair(v_con[account]->td->m_RspUserLogin.FrontID, make_pair(v_con[account]->td->m_RspUserLogin.SessionID, ref));
                    if(orders.find(order) != orders.end())
                        v_con[account]->td->ReqOrderAction(&orders[order]);
                    else
                        mexWarnMsgTxt("δ���ڴ˱���\n");
                }
            }
            else if(nrhs == 5)
            {
                int frontid = (int)mxGetScalar(prhs[2]);
                int session = (int)mxGetScalar(prhs[3]);
                string ref = mxArrayToString(prhs[4]);
                pair<int, pair<int, string> > order =
                        make_pair(frontid, make_pair(session, ref));
                if(orders.find(order) != orders.end())
                    v_con[account]->td->ReqOrderAction(&orders[order]);
                else
                    mexWarnMsgTxt("δ���ڴ˱���\n");
            }
            
            break;
        }
        
        //��ȡ���гֲ���Ϣ
        case 9:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            // ��ѯ���гֲ�
            if(nrhs == 2)
                plhs[0] = GetPositionData(v_con[account]->ctpData->GetPosition());
            // ��ѯָ���ֲ�
            else if(nrhs == 3)
                plhs[0] = GetPositionData(v_con[account]->ctpData->GetPosition(), mxArrayToString(prhs[2]));
            break;
        }
        //�ж��Ƿ�����
        case 10:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            bool isconnect = !(NULL == v_con[account]);
            plhs[0] = mxCreateLogicalScalar(isconnect);
            break;
        }
        
        //��ѯ������Ϣ
        case 11:
        {
            int account = (int)mxGetScalar(prhs[1]);
            CheckIsConnect(account);
            plhs[0] = GetErrorInfo(v_con[account]->ctpData->GetErrorInfo());
            break;
        }
        default:
            mexErrMsgTxt("û���ҵ���ز���");
            
    }
    
}
