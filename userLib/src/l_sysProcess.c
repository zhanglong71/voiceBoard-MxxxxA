#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"
#include "version.h"
#include "l_arch.h"
#include "l_sysProcess.h"
#include "l_actionFIFO.h"
#include "l_u8FIFO.h"
#include "l_uart.h"
#include "l_rs485.h"
#include "l_jsonTL.h"

#include <stdlib.h>
#include <string.h>

int sysProcess(void *pMsg)
{
//    msg_t msg;
    int iRet = TRUE;
    u8Data_t u8Data;
    int flag;
    int i;
    int voi = 0;
    
    switch(((msg_t *)pMsg)->msgType)
    {
    case CMSG_UART1RX:
        // test only ?????????????????????
        //msg.msgType = CMSG_TEST;
        //msgq_in_irq(&g_msgq, &msg);
        // ???????????????????????????????
        break;

    case CMSG_UART2RX:  // command data from homonary(wifi)
        if(u8FIFOisEmpty(&g_uart2RxQue) != TRUE) {
            /** do something to uart2 data **/
            objType_t objtype = sm_receiveData(g_buf);
            if ((objtype == obj_SSID) || (objtype == obj_IP) || (objtype == obj_MAC)) {
                memset(g_KVarr, 0, sizeof(g_KVarr));
                strim(g_buf);
                if (JsonParseL0(g_buf, g_KVarr) == 0) {
                    /** do what? nothing! **/
                }
                for (i = 0; ((i < MTABSIZE(g_KVarr)) && (g_KVarr[i].KVIndex > 0)); i++) {
                    if (strstr(g_KVarr[i].key, "status") && strstr(g_KVarr[i].value, "1")) {
                        flag |= (1 << 1);
                    }
                    if (strstr(g_KVarr[i].key, "ssid")) {
                        flag |= (1 << 2);
                        strncpy(g_netInfo.ssid, g_KVarr[i].value, sizeof(g_netInfo.ssid));
                    }
                    if (strstr(g_KVarr[i].key, "ip")) {
                        flag |= (1 << 2);
                        strncpy(g_netInfo.ip, g_KVarr[i].value, sizeof(g_netInfo.ip));
                    }
                    if (strstr(g_KVarr[i].key, "mac")) {
                        flag |= (1 << 2);
                        strncpy(g_netInfo.mac, g_KVarr[i].value, sizeof(g_netInfo.mac));
                    }
                }
                if ((flag & 0x60) == 0x60) { /** make sure the status==1 **/
                    g_netInfo.flag |= (1 << 0);
                }
            } else if (objtype == obj_RSSI) {
                strncpy(g_netInfo.rssi, g_buf, sizeof(g_netInfo.rssi));
                g_netInfo.flag |= (1 << 3);
            } else if (objtype == obj_PUTCHAR) {
                memset(g_KVarr, 0, sizeof(g_KVarr));
                if (JsonParseL1(g_buf, g_KVarr) == POK) {  // ????????????????????????????????
                    /** do what? nothing! **/
                }
                //if (strstr(g_KVarr[i].key, "switch")) {
                //    g_componentStatus.voicePrompt = atoi(g_KVarr[i].value);
                //}
                AckPutCharStatusByMsgType(((msg_t *)pMsg)->msgType);
            } else { /** (objtype == obj_key) || (objtype == obj_len) || (objtype == obj_body) || (objtype == obj_none) **/
                /** do nothing **/
            }
        }
        break;
 
    // case CUART1_TOUT:
    case CMSG_UART1RTOUT:
        /************************************
         * receive: {"xxxx":voi,"PLY":xxx,"SEQ":num}
         * ack:
         *    {"voi":xxxx,"PLY":OK,"SEQ":num}
         *    {"voi":xxxx,"PLY":err,"SEQ":num}
         *
         * receive: {"user":voi,"ask":ver}
         * ack:     {"voi":user,"ver":v001}
         ************************************/
        memset(g_buf, 0, sizeof(g_buf));
        memset(g_KVarr, 0, sizeof(g_KVarr));

        for(i = 0; ((i < MTABSIZE(g_buf)) && (u8FIFOout_irq(&g_uart1RxQue, &u8Data) == TRUE)); i++) {
            g_buf[i] = u8Data.u8Val;
        }
        if (i < MTABSIZE(g_buf)) {
            g_buf[i] = '\0';
        } else {
            u8FIFOinit_irq(&g_uart1RxQue);  /** !!!!!!!!!!!! **/
            /** something wrong happened **/
        }

        if (JsonParseL0(g_buf, g_KVarr)) {
            u8 flag = 0;
            u8 src_idx = 0;
            u8 u8Seq = 0;
            static u8 u8Seq_last = 0xff;
            //u8 voi_idx = 0;
            for (u8 i = 0; ((i < MTABSIZE(g_KVarr)) && (g_KVarr[i].KVIndex > 0)); i++) {
                 if (strstr(g_KVarr[i].value, "voi")) {
                     Mset_bit(flag,1);
                     src_idx = i;
                 }
                 if (strstr(g_KVarr[i].key, "PLY")) {
                     Mset_bit(flag,2);
                     voi = atoi(g_KVarr[i].value);
                 }
                 if (strstr(g_KVarr[i].key, "SEQ")) {
                     Mset_bit(flag,3);
                     u8Seq = atoi(g_KVarr[i].value);
                 }
                 if (strstr(g_KVarr[i].key, "ask") && strstr(g_KVarr[i].value, "ver")) {
                     Mset_bit(flag,4);
                 }
                 if (strstr(g_KVarr[i].key, "bat_v")) {  // !!!!!! The data takes effect immediately !!!!!!
                     Mset_bit(flag,5);
                     g_componentStatus.bat_v = atoi(g_KVarr[i].value);
                 }
            }
            if (Mget_bit(flag, 1) && Mget_bit(flag, 2) && Mget_bit(flag, 3)) {
                if (voi < 100) {
                    if (u8Seq_last != u8Seq) {
                        u8Seq_last = u8Seq;
                        vp_stop1();
                        if ((g_componentStatus.voicePrompt == CINDEX_VOICEPROMPT_OFF) && 
                            (!((voi == vopIdx_VoiceOpen) || (voi == vopIdx_VoiceClose)))) {
                            /** shutdown voice, do nothing **/
                        } else {
                            vp_stor(voi);
                        }
                    }
                    setStatusByvoiceIdx((u8)voi);
                    setCommonFaultStatusByComponent();
                    reportStatusByvoiceIdx((u8)voi);
                    /** construct ack**/
                    generateVoiceAckOk(g_KVarr[src_idx].key, u8Seq_last);
                } else {
                    generateVoiceAckErr(g_KVarr[src_idx].key, u8Seq_last);
                }
            }
            
            if (Mget_bit(flag, 1) && Mget_bit(flag, 4)) {
                generateVoiceAckVer(g_KVarr[src_idx].key, CVERSION);
            }
        }
        break;

    case CMSG_VOPOFF:
    case CMSG_VOPON:
        // u8 voi_idx;
        setStatusByMsgType(((msg_t *)pMsg)->msgType);
        getVoiIdxByMsgType(((msg_t *)pMsg)->msgType, (u8*)&voi);
        vp_stop1();
        vp_stor((u8)voi);
        break;

    case CDISCONN_CLOUD:
    case CCONN_CLOUD:
        setStatusByMsgType(((msg_t *)pMsg)->msgType);
        break;
        
    case CMSG_UART2TX:
        sm_sendData(NULL);
        break;
        
    // case CGETCHAR_MOP:
    // case CGETCHAR_ROLLER:
    case CGETCHAR_CLEANWATERBOXSTATE:
    // case CGETCHAR_PUMP:
    // case CGETCHAR_CHARGE:
    case CGETCHAR_STATUS:
    case CGETCHAR_VOICEPROMPT:
    case CGETCHAR_COMMONFAULTDETECTION:
        AckgetCharStatusByMsgType(((msg_t *)pMsg)->msgType);
        break;
        
    case CGETCHAR_BATTERY:
        ackgetCharBattery();
        break;
    case CGETCHAR_NETINFO:
        checkAndAckGetCharNetInfo();
        break;
    case CGETCHAR_UPDATE:
        checkAndAckGetCharUpdate();
        break;
    case CPUT_SYNC:
        (void)reportAckPutSync();
        break;
    
    // case CPUT_CHAR:
    //    break;
        
    case CGETDEVINFO_REQ:
    case CGETDEVINFO_RSPERROR:
        (void)reportDevInfo(NULL);
        break;
        
    case CREPORT_RSPERROR:   /** report Services error, try again **/
    case CGETDEVINFO_RSPOK:  /** according protocal report Services after devInfo reported ok **/
        (void)reportService(0);
        break;

    case CSETCMDINTERVAL_RSPOK:
        /** do nothing **/
        reportGetCmdInterval();
        break;

    case CSETCMDINTERVAL_RSPERROR:
        /** do nothing **/
        break;

    case CREPORT_RSPOK:
        /** do nothing or do something here **/
        (void)reportSetCmdInterval();
        break;

    case CHEART_BEAT:
        (void)reportHeartbeat();
        break;

    case CWIFI_TEST:
        /** do test only! 2-byte data !!! **/
        break; 

    case CPMT_TOUT:
		actProcess(&g_promptQueue);
		break;
        
    case CMSG_485STEP:
        rs485actProcess();
        break;
    
    case CMSG_485OVER:    
        rs485actOver();
        break;
    
    default:
        iRet = FALSE;
        break;	
	  }
    return  iRet;   
}

void deamon_task(void)
{
    // deamon_485uart1_send();
    rs485TransXDoing();
    deamon_uart2_send();
    actionDoing(&g_promptQueue);
}
/**************************************************************************************************/

/**************************************************************************************************/
void netInfoData_init(void)
{
    #if 1
    g_netInfo.flag = 0;

    strcpy(g_netInfo.rssi, "-13");
    strcpy(g_netInfo.ssid, "DIISEA-ssid");
    strcpy(g_netInfo.ip,  "10.23.45.67");
    strcpy(g_netInfo.mac, "ab:cd:ef:01:23:45");
    #endif
}

/** Ack for query; no matter status changed or not **/
void checkAndAckGetCharNetInfo(void)
{
    (void)reportgetCharNetInfo(&g_netInfo);
}
/**************************************************************************************************/
void checkAndAckGetCharUpdate(void)
{
    (void)getCharAckComponentStatus(CINDEX_UPDATE);
}

/*****************************************************************************/
/**
 * ַ¿ذ吨ҪϲӯҴ°带֪µ±ǰ¹¤׷״̬! µ±ǰ·½°¸ʇֱ½ӓÖ÷¿ذ巢À´µē±຅׷Ϊ¹¤׷״̬ͨ¸捊 **/

Triplet_u8u8pu8_t const voiceIdx2status[] = {
    // {vopIdx_ConnectNo, CINDEX_CHARGING, &(g_componentStatus.mop)},   //=1, //δÁ¬½Ӊ豸//¹²Ӄ//
    // {vopIdx_Disconnect, CINDEX_CHARGING, &(g_componentStatus.mop)},  // =2,//ɨ±¸ґ¶Ͽª
    // {vopIdx_Install, CINDEX_CHARGING, &(g_componentStatus.mop)},  // =3,//½øȫɨփģʽ
    {vopIdx_VoiceOpen, CINDEX_VOICEPROMPT_ON,  &(g_componentStatus.voicePrompt)},  // =4,//¿ªƴӯҴ
    {vopIdx_VoiceClose, CINDEX_VOICEPROMPT_OFF, &(g_componentStatus.voicePrompt)},  // =5,//¹رՓ
    // {vopIdx_WifiReset, CINDEX_CHARGING, &(g_componentStatus.mop)},  // =6,//͸§¸´λ³ɹ¦
    // {vopIdx_WifiConnecting, CINDEX_CHARGING, &(g_componentStatus.mop)},  // =7,//͸§սԚÁ¬½Ӎ
    // {vopIdx_WifiOk, CINDEX_CHARGING, &(g_componentStatus.mop)},  // =8,//͸§Á¬½ӳɹ¦
    
    {vopIdx_CHing, CINDEX_CHARGING, &(g_componentStatus.charge)},  // =9,//¿ªʼ³䵧
    {vopIdx_CHcomplete, CINDEX_CHARGECOMPLETE, &(g_componentStatus.charge)},  // =10,//³䵧ґͪ³ɍ
    {vopIdx_Choff, CINDEX_UNCHARGED, &(g_componentStatus.charge)},  // =11,//³䵧֐¶ύ
    
    {vopIdx_standard, CINDEX_STANDARD, &(g_componentStatus.status)},  // =12,//½øȫ±ꗼģʽ
    {vopIdx_RUNm2, CINDEX_HIGHPOWER, &(g_componentStatus.status)},  // =13,//½øȫǿÁ¦ģʽ
    // {vopIdx_nop2, CINDEX_STANDBY, &(g_componentStatus.status},  // =14,//´󋮳叴ģʽ
    {vopIdx_RUNCL, CINDEX_CLEANING, &(g_componentStatus.status)},  // =15,//½øȫהǥϴģʽ
    {vopIdx_RunclOver, CINDEX_STANDBY, &(g_componentStatus.status)},  // =16,//הǥϴґͪ³ɍ
    
    {vopIdx_RUNover, CINDEX_STANDBY, &(g_componentStatus.status)},  // =17,//ԋА½ኸ£¬ǫ·Żصחùהǥϴ	
    
    // {vopIdx_RUNOFF, CINDEX_STANDBY, &(g_componentStatus.status)},  // =18,//ԋА½ኸ
    {vopIdx_Chlowing, CINDEX_BATTERYLOW,  &(g_componentStatus.battery)},  // =19,//µ灿²»ף£¬ǫ¼°ʱ³䵧
    // {nop3, CINDEX_BATTERYLOW, &(g_componentStatus.battery)},  // =20,//µ灿²»ף£¬ǫÁ¢¼´³䵧
    // {nop4, CINDEX_BATTERYLOW, &(g_componentStatus.battery)},  // =21,//µ灿²»ף£¬ǫ³䵧º󼌐ø
    {vopIdx_CHErr, CINDEX_CHARGEFAULT, &(g_componentStatus.charge)},  // =22,//³䵧Ҭ³££¬ǫ¼첩³䵧Ʒ
    //{vopIdx_ConnectDragLala, CINDEX_STANDBY, &(g_componentStatus.battery)},  // =23,//ϴµػúґÁ¬½Ӎ
    //{vopIdx_CisternOk, CINDEX_STANDBY, &(g_componentStatus.battery)},  // =24,//ˮϤґ°²װ
    //{vopIdx_CisternNo, CINDEX_STANDBY, &(g_componentStatus.battery)},  // =25,//ˮϤґȡ³ö
    //{vopIdx_CisternTake,CINDEX_STANDBY,  &(g_componentStatus.battery)},  // =26,//ˮϤґȡ³ö£¬½øȫ´󋮳叴ģʽ
    {vopIdx_sewageErr, CINDEX_CLEANWATERSHORTAGE, &(g_componentStatus.cleanWater)},  // =27,//ΛˮϤґº£¬ǫǥÀ펛ˮϤ
    {vopIdx_ClearErr, CINDEX_CLEANWATERSHORTAGE, &(g_componentStatus.cleanWater)},  // =28,//ǫ¼ӈ뇥ˮ
    // {vopIdx_PumpErr, CINDEX_STANDBY, &(g_componentStatus.clearWater)},  // =29,//ˮ±õ绺Ҭ³£
    // {nop5, CINDEX_STANDBY,  &(g_componentStatus.clearWater)},  // =30,//ˮ±õ绺δ°²װ
    
    {vopIdx_RollerErr, CINDEX_ROLLEROVERLOAD, &(g_componentStatus.roller)},  // =31,//ǫ¼첩¹öͲ
};
/*****************************************************************************/
RetStatus setStatusByvoiceIdx(u8 idx)
{
    int i = 0;
    for (i = 0; i < MTABSIZE(voiceIdx2status); i++) {
        if (voiceIdx2status[i].idx == idx) {
            *(voiceIdx2status[i].ptr) = voiceIdx2status[i].status_idx;
            return POK;
        }
    }
    return PERROR;
}

RetStatus reportStatusByvoiceIdx(u8 idx)
{
    int i = 0;
    for (i = 0; i < MTABSIZE(voiceIdx2status); i++) {
        if (voiceIdx2status[i].idx == idx) {
            reportComponentStatus(voiceIdx2status[i].status_idx);
            return POK;
        }
    }

    return PERROR;
}

RetStatus setCommonFaultStatusByComponent(void)
{
    if (g_componentStatus.roller == CINDEX_ROLLEROVERLOAD) {
        g_componentStatus.commonFaultDetection = g_componentStatus.roller;
        return POK;
    } else if (g_componentStatus.pump == CINDEX_PUMPCURRENTSMALL) { /** D7 has no this feature(ability) **/
        g_componentStatus.commonFaultDetection = g_componentStatus.pump;
        return POK;
    } else if (g_componentStatus.charge == CINDEX_CHARGEFAULT) {
        g_componentStatus.commonFaultDetection = g_componentStatus.charge;
        return POK;
    }

    g_componentStatus.commonFaultDetection = CINDEX_NODEFAULT;
    return POK;
}

/*****************************************************************************/
static const pair_msgType2u8ptr_t msgType2u8ptr[] = {
    {CGETCHAR_ROLLER,             &(g_componentStatus.roller)},
    {CGETCHAR_PUMP,               &(g_componentStatus.pump)},
    {CGETCHAR_CLEANWATERBOXSTATE, &(g_componentStatus.cleanWater)},
    {CGETCHAR_BATTERY,            &(g_componentStatus.battery)},
    {CGETCHAR_CHARGE,             &(g_componentStatus.charge)},
    {CGETCHAR_STATUS,             &(g_componentStatus.status)},
    {CGETCHAR_VOICEPROMPT,        &(g_componentStatus.voicePrompt)},
    {CGETCHAR_COMMONFAULTDETECTION,  &(g_componentStatus.commonFaultDetection)},
    
    // {CGETCHAR_NETINFO,    &(g_componentStatus.charge)},
    // {CGETCHAR_UPDATE,     &(g_componentStatus.charge)},
};

RetStatus AckgetCharStatusByMsgType(msgType_t msgType)
{
    int i = 0;
    for (i = 0; i < MTABSIZE(msgType2u8ptr); i++) {
        if (msgType2u8ptr[i].first == msgType) {
            getCharAckComponentStatus(*(msgType2u8ptr[i].second));
            return POK;
        }
    }
    return PERROR;
}

/*****************************************************************************/
static const Triplet_msgTypeu8pu8_t msgType2u8u8ptr[] = {
    {CMSG_VOPON,  CINDEX_VOICEPROMPT_ON,       &(g_componentStatus.voicePrompt)},
    {CMSG_VOPOFF, CINDEX_VOICEPROMPT_OFF,      &(g_componentStatus.voicePrompt)}, 

    {CDISCONN_CLOUD, CINDEX_NETCONNECTION_OFF, &(g_componentStatus.netConnection)},
    {CCONN_CLOUD,    CINDEX_NETCONNECTION_ON,  &(g_componentStatus.netConnection)},
};

RetStatus setStatusByMsgType(msgType_t msgType)
{
    int i = 0;
    for (i = 0; i < MTABSIZE(msgType2u8u8ptr); i++) {
        if (msgType2u8u8ptr[i].msgType == msgType) {
            *(msgType2u8u8ptr[i].ptr) = msgType2u8u8ptr[i].status_idx;
            return POK;
        }
    }
    return PERROR;
}

const pair_u8msgType_t msgType2voiceIdx[] = {
    {vopIdx_VoiceOpen,  CMSG_VOPON},
    {vopIdx_VoiceClose, CMSG_VOPOFF},
};

RetStatus getVoiIdxByMsgType(const msgType_t msgType, u8* voi_idx)
{
    for (int i = 0; i < MTABSIZE(msgType2voiceIdx); i++) {
        if (msgType2voiceIdx[i].second == msgType) {
            *voi_idx = msgType2voiceIdx[i].first;
            return POK;
        }
    }
    return PERROR;
}

RetStatus AckPutCharStatusByMsgType(msgType_t msgType)
{
    return POK;
}

/*****************************************************************************/
