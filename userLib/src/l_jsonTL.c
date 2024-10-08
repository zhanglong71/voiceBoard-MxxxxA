
#include "CONST.h"
#include "ptype.h"
#include "macro.h"
#include "global.h"

#include "l_arch.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "l_u8FIFO.h"
#include "l_jsonTL.h"
#include "l_timer.h"
#include "l_gpio.h"
#include "l_jsonTL.h"
#include "l_rs485.h"

RetStatus reportDevInfo(unsigned *arg)
{
    (void)arg;
    jsonTL_t* p = getDevInfo(0);
    sm_sendData(p);
    return (POK);
}

RetStatus reportService(unsigned *arg)
{
    (void)arg;
    jsonTL_t* p = getService(0);
    sm_sendData(p);
    return (POK);
}

RetStatus reportNobodyInfo(char* data, int len)
{
    u8Data_t u8Data;
    if ((data == NULL) || (len <= 0)) {
        return PINVAL;
    }
    for (int i = 0; ((i < strlen(data)) && (i < len)); i++) {
        u8Data.u8Val = data[i];
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
    }
    return (POK);
}

RetStatus reportHeartbeat(void)
{
    char buf[] = "heartbeat,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

RetStatus reportResetNet(void)
{
    char buf[] = "resetNet,2,AP\n";
    return reportNobodyInfo(buf, strlen(buf));
}

RetStatus reportScanWifi(void)
{
    char buf[] = "scanWifi,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

RetStatus reportAckPutSync(void)
{
    char buf[] = "putSync,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

/** set interval time(ms) **/
RetStatus reportSetCmdInterval(void)
{
    char buf[] = "setCmdInterval,2,70\n";
    return reportNobodyInfo(buf, strlen(buf));
}

/** get interval time(ms) **/
RetStatus reportGetCmdInterval(void)
{
    char buf[] = "getCmdInterval,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

static const reportStatusBody_t reportStatusBodyArr[] = {
    { CINDEX_STANDBY,             "{\"status\":{\"status\":0}}"},              // standby
    { CINDEX_STANDARD,            "{\"status\":{\"status\":1}}"},              // standard
    { CINDEX_HIGHPOWER,           "{\"status\":{\"status\":2}}"},              // highPower
    { CINDEX_CLEANING,            "{\"status\":{\"status\":3}}"},               // shelf clear
 /****/   
    { CINDEX_PUMPNORMAL,          "{\"commonFaultDetection\":{\"code\":107}}"},               // pumpNormal
    { CINDEX_PUMPOVERLOAD,        "{\"commonFaultDetection\":{\"code\":106}}"},               // pumpOverload
    
    { CINDEX_ROLLERNORMAL,        "{\"commonFaultDetection\":{\"code\":105}}"},               // roller error recover to normal
    { CINDEX_ROLLEROVERLOAD,      "{\"commonFaultDetection\":{\"code\":104}}"},               // roller error
    
    { CINDEX_PUMPNORMAL      ,    "{\"commonFaultDetection\":{\"code\":103}}"},               // pump current is too small recover to normal
    { CINDEX_PUMPCURRENTSMALL,    "{\"commonFaultDetection\":{\"code\":102}}"},               // pump current is too small
    
    { CINDEX_CHARGEREPAIR,        "{\"commonFaultDetection\":{\"code\":101}}"},               // charging fault recover to normal
    { CINDEX_CHARGEFAULT,         "{\"commonFaultDetection\":{\"code\":100}}"},               // charging fault
    { CINDEX_NODEFAULT,           "{\"commonFaultDetection\":{\"code\":0}}"},               // no fault
 /****/   
    { CINDEX_CLEANWATERNORMAL,    "{\"cleanwaterboxstate\":{\"Cleanwaterboxstate\":0}}"},  // clear water normal
    { CINDEX_CLEANWATERSHORTAGE,  "{\"cleanwaterboxstate\":{\"Cleanwaterboxstate\":1}}"},  // clear water shortage
    
    { CINDEX_UNCHARGED,           "{\"battery\":{\"charging\":0}}"},           // Not Charging
    { CINDEX_CHARGING,            "{\"battery\":{\"charging\":1}}"},           // Charging
    { CINDEX_CHARGECOMPLETE,      "{\"battery\":{\"charging\":100}}"},         // Charge Complete
    
    { CINDEX_BATTERYNORMAL,       "{\"battery\":{\"alarm\":0}}"},              // Normal battery level
    { CINDEX_BATTERYLOW,          "{\"battery\":{\"alarm\":1}}"},              // Low voltage(Low battery)

    { CINDEX_BATTERY,             "{\"battery\":{\"alarm\":%u,\"charging\":%u,\"level\":%u}}"},
    
    { CINDEX_VOICEPROMPT_ON,      "{\"voicePrompt\":{\"switch\":1}}"},         // voice prompt on
    { CINDEX_VOICEPROMPT_OFF,     "{\"voicePrompt\":{\"switch\":0}}"},         // voice prompt off

    { CINDEX_NETINFO,             "{\"netInfo\":{\"IP\":%s,\"RSSI\":%s,\"SSID\":%s}}"},
    { CINDEX_UPDATE,              "{\"update\":{\"versoin\":1.0.1,\"introduction\":newest,\"progress\":100,\"bootTime\":60}}"},
};

void ackgetCharBattery(void)
{
    jsonTL_t jsonTypeTx;
    char buf[U8FIFOSIZE]; 
    u8 idx = 0;
    u8 charging = 0;
    u8 batteryLow = 0;
    u8 batteryLevel = batteryVoltage2percent(g_componentStatus.bat_v);

    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == CINDEX_BATTERY) {
            break;
        }
    }
    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return;
    }

    if (g_componentStatus.charge == CINDEX_CHARGING) {
        charging = 1;
    }
    
    if (g_componentStatus.battery == CINDEX_BATTERYLOW) {
        batteryLow = 1;
    }

    jsonTypeTx.jHead = "getChar";
    sprintf(buf, reportStatusBodyArr[idx].body, batteryLow, charging, batteryLevel);
    jsonTypeTx.jBody =buf;
    jsonTypeTx.jLen = strlen(jsonTypeTx.jBody);

    sm_sendData_once(&jsonTypeTx);
}

void reportBattery(void)
{
    jsonTL_t jsonTypeTx;
    char buf[U8FIFOSIZE]; 
    u8 idx = 0;
    u8 charging = 0;
    u8 batteryLow = 0;
    u8 batteryLevel = batteryVoltage2percent(g_componentStatus.bat_v);

    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == CINDEX_BATTERY) {
            break;
        }
    }
    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return;
    }

    if (g_componentStatus.charge == CINDEX_CHARGING) {
        charging = 1;
    }
    
    if (g_componentStatus.battery == CINDEX_BATTERYLOW) {
        batteryLow = 1;
    }

    jsonTypeTx.jHead = "reportChar";
    sprintf(buf, reportStatusBodyArr[idx].body, batteryLow, charging, batteryLevel);
    jsonTypeTx.jBody =buf;
    jsonTypeTx.jLen = strlen(jsonTypeTx.jBody);

    sm_sendData_once(&jsonTypeTx);

}

RetStatus reportgetCharNetInfo(NetInfo_t* netInfo)
{
    jsonTL_t jsonTypeTx;
    char buf[U8FIFOSIZE]; 
    u8 idx = 0;

    if (netInfo == NULL) {
        return (PERROR);
    }
    
    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == CINDEX_NETINFO) {
            break;
        }
    }
    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return (PERROR);
    }

    jsonTypeTx.jHead = "getChar";
    sprintf(buf, reportStatusBodyArr[idx].body, netInfo->ip, netInfo->rssi, netInfo->ssid);

    jsonTypeTx.jBody = buf;
    jsonTypeTx.jLen = strlen(jsonTypeTx.jBody);

    sm_sendData_once(&jsonTypeTx);

    return (POK);
}

/***********************************************************************
 * moto/pump/battery/clear/charge
 ***********************************************************************/
int reportComponentStatus(u8 statusIndex)
{
    static jsonTL_t jsonTypeTx;
    u8 idx = 0;

    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == statusIndex) {
            break;
        }
    }

    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return (MTABSIZE(reportStatusBodyArr));
    }

    jsonTypeTx.jHead = "reportChar";
    jsonTypeTx.jBody = reportStatusBodyArr[idx].body;
    jsonTypeTx.jLen = strlen(jsonTypeTx.jBody);
    
    sm_sendData_once(&jsonTypeTx);
    return 0;
}

int getCharAckComponentStatus(u8 statusIndex)
{
    static jsonTL_t jsonTypeTx;
    u8 idx = 0;

    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == statusIndex) {
            break;
        }
    }

    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return (MTABSIZE(reportStatusBodyArr));
    }

    jsonTypeTx.jHead = "getChar";
    jsonTypeTx.jBody = reportStatusBodyArr[idx].body;
    jsonTypeTx.jLen = strlen(jsonTypeTx.jBody);
    
    sm_sendData_once(&jsonTypeTx);
    return 0;
}

/*********************************************************************/
jsonTL_t* getDevInfo(u8 idx)
{
    static jsonTL_t jsonTypeDevInfo[] = {
        {
            "getDevInfo", 0,
            "{\"v\":\"1.0.0\","
            "\"dv\":\"1.0.0\","
            "\"prodId\":\"2NPQ\","
            "\"deviceTypeId\":\"19F\","
            "\"manufacturerId\":\"hlp\","
            "\"deviceModel\":\"D7\","
            "\"deviceTypeNameEn\":\"Scrubber\","
            "\"manufacturerNameEn\":\"DIISEA\","
            "\"networkType\":\"AP\","
            "\"acKey\":\"2B5F3377287C4920506E604B326D5A6479F44A6942B1FE3C86CAD3E3A5F9654D6BC810E9D216466D843A0385A723CC8E\","
            "\"productSeries\":\"DM\","
            "\"productKey\":\"292a6ce5e37b4f3d823990d7370b7094\","
            "\"marketName\":\"滴水洗地机D7\","
            //"\"marketName\":\"DIISEA-D7\","
            "\"brand\":\"DIISEA\"}"
        },
    };

    if (idx >= MTABSIZE(jsonTypeDevInfo)) {
        return (NULL);
    }
	return (&jsonTypeDevInfo[idx]);
}

jsonTL_t* getService(u8 idx)
{
    static jsonTL_t ServiceArr[] = {
        {
            "reportService", 0,
            "{"
            #if 1
                "\"sId\":["
                    "\"cleanwaterboxstate\","
                    "\"battery\","
                    "\"status\","
                    "\"commonFaultDetection\","
                    "\"voicePrompt\""
                    "],"
                "\"sType\":["
                    "\"status\","
                    "\"battery\","
                    "\"status\","
                    "\"fault\","
                    "\"voicePrompt\""
                "]"
            #endif
            "}"
        }
    };

    if (idx >= MTABSIZE(ServiceArr)) {
        return (NULL);
    }
    return (&ServiceArr[idx]);
}

/**
 * ��sprintf()ת�����ֵ��ַ���ʱ���ֹ���! ԭ��δ֪! �ô˺������
 **/
RetStatus digit2ascii(int from, char* to)
{
    if ((from > 999) || (from < 0) || (to == NULL)) {
        return PERROR;
    }
    #if 1
    if (from > 99) {
        to[0] = '0' + (from / 100);
        to[1] = '0' + ((from / 10) % 10);
        to[2] = '0' + (from % 10);
        to[3] = '\0';
    } else if (from > 9) {
        to[0] = '0' + (from / 10);
        to[1] = '0' + (from % 10);
        to[2] = '\0';
    } else {
        to[0] = '0' + from;
        to[1] = '\0';
    }
    #endif
    return POK;
}

/**
 * remove the first and last blank character
 **/
RetStatus strim(char* str) 
{
    char *end, *sp, *ep;
    int len;
    
    sp = str;
    end = str + strlen(str) - 1;  /** the last valid character **/ 
    ep = end;
    
    while((sp < end) && isspace(*sp)) {     /** head blank **/
        sp++;
    }
    while((ep >= sp) && isspace(*ep)) {      /** tail blank **/
        ep--;
    }
    len = ((ep < sp) ? 0:((ep - sp) + 1));  /** valid character length **/

    if (str != sp) {
        for (int i = 0; i < len; i++) {
            str[i] = sp[i];
        }
    }
    str[len] = '\0';
    return POK;
}

/**
 * aaaaaaXbbbbbbYccccccc
 * ==> strip(abc, '{', '}') ==>
 * bbbbbb
 *
 * str�ַ����У�ֻ����������hch�����ҵ�tch֮���
 **/
char strip(char* str, char hch, char tch) 
{
    char* hp = NULL;
    char* tp = NULL;
    u8 i = 0;
    
    if((hp = strchr(str, hch)) == NULL) {   /** Search from the head ==> **/
        return (0);
    }

    if((tp = strrchr(str, tch)) == NULL) {  /** Search from the tail <== **/
        return (0);
    }
    if(tp < hp) {
        return (0);
    }
    hp++;
    *tp = '\0';
    while((str[i] = hp[i]) != '\0') {
        i++;
    }

    return (1);
}

u8 batteryVoltage2percent(u16 voltage)
{
#define CVOLTAGE_MAX (1580)
#define CVOLTAGE_MIN (1300)
    unsigned int tmp;
    if (voltage >= CVOLTAGE_MAX) {
        return 100;
    } else if (voltage <= CVOLTAGE_MIN) {
        return 0;
    }
    tmp = (voltage - CVOLTAGE_MIN) * 100 / (CVOLTAGE_MAX - CVOLTAGE_MIN);
    return (u8)tmp;
}

/**
 * send all data at once 
 * make sure the length of string is less than length of FIFO buff
 **/
void sm_sendData_once(jsonTL_t* jp)
{
    u8Data_t u8Data;
    char len_buf[8];
    int len = 0;
    int send_count = 0;
    // int i = 0;

    if (jp == NULL) {
        return;
    }

    /** hhhhhhhhh head **/
    len = strlen(jp->jHead);
    for (int i = 0; i < len; i++) {
        u8Data.u8Val = jp->jHead[i];
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
    }
    u8Data.u8Val = ',';
    u8FIFOin_irq(&g_uart2TxQue, &u8Data); 

    /** lllllllll length **/
    len = strlen(jp->jBody);
    if (strchr(jp->jBody, '\n')) {
        len--;
    }
    #if 0
    if (sprintf(len_buf, "%d", len)) {
    #else
    if (digit2ascii(len, len_buf) == POK) {
    #endif
        for (int i = 0; i < strlen(len_buf); i++) {
            u8Data.u8Val = len_buf[i];
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        }
    }

    if (len > 0) {
        u8Data.u8Val = ',';
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
    } else { /** len == 0, nobody need transmit **/
        u8Data.u8Val = '\n';
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        return;
    }
    /** bbbbbbbbb body **/
    for (send_count = 0; ((send_count < len) && (send_count < Mu8FIFO_bufLen(&g_uart2TxQue))); send_count++) {
        u8Data.u8Val = jp->jBody[send_count];
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
    }

    if (send_count >= len) {               /** the last transmit part **/
        u8Data.u8Val = '\n';
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
    }
}

/**
 * state machine
 *
 * sm_init =(jp!=NULL)=> sm_step =(p!=NULL)=> sm_end ==>sm_init
 * 
 * sm_init: send data include head+len+head of body(< 40 byte)
 * sm_step: send data body(< 60 byte) again and again(CMSG_UART3TX)
 * sm_end: send over
 *
 * note: send data the length of g_uart2TxQue buf
 **/
void sm_sendData(jsonTL_t* jp)
{
    static smStatus_t s_smStatus = sm_init;
    static jsonTL_t* p = NULL;
    static u16 total = 0;
    u8Data_t u8Data;
    char buf[8];
    int len = 0;

    if (s_smStatus == sm_init) {   /** first enter the send process **/
        if (jp == NULL) {
            return;
        }
        s_smStatus = sm_step;
        p = jp;
        total = 0;
        
        /** hhhhhhhhh head **/
        len = strlen(p->jHead);
        for (int i = 0; i < len; i++) {
            u8Data.u8Val = p->jHead[i];
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        }
        u8Data.u8Val = ',';
        u8FIFOin_irq(&g_uart2TxQue, &u8Data); 
        
        /** lllllllll length **/
        len = strlen(p->jBody);
        if (strchr(p->jBody, '\n')) {
            len--;
        }
        if (sprintf(buf, "%d", len)) {
            for (int i = 0; i < strlen(buf); i++) {
                u8Data.u8Val = buf[i];
                u8FIFOin_irq(&g_uart2TxQue, &u8Data);
            }
        }
        
        if (len > 0) {
            u8Data.u8Val = ',';
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        } else { /** len == 0, nobody need transmit **/
            u8Data.u8Val = '\n';
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
            s_smStatus = sm_init;
            return;
        }
        /** bbbbbbbbb body **/
        for (int i = 0; ((i < len) && (i < (Mu8FIFO_bufLen(&g_uart2TxQue) - 32))); i++, total++) {
        // for (int i = 0; ((i < len) && (Mu8FIFO_bufLen(&g_uart2TxQue) > u8FIFOlength(&g_uart2TxQue))); i++, total++) {
            u8Data.u8Val = p->jBody[i];
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        }
    } else if (s_smStatus == sm_step) {   /**  enter the send process again **/
        /** body **/
        if (p == NULL) {
            s_smStatus = sm_end;
            return; 
        }
        len = strlen(p->jBody);
        for (int i = 0; ((total < len) && (i < Mu8FIFO_bufLen(&g_uart2TxQue) - 1)); i++, total++) {
            u8Data.u8Val = p->jBody[total];
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
        }

        if (total >= len) {               /** the last transmit part **/
            u8Data.u8Val = '\n';
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
            s_smStatus = sm_end;
        }
    } else if (s_smStatus == sm_end) {   /** transmit over **/
        s_smStatus = sm_init;
        p = NULL;
    } else { /** unrecognize **/
        s_smStatus = sm_init;
        p = NULL;
    }
}

/********************************
 * enrolled key 
 * 
 * Correspondence between index and string(key)
 ********************************/
 const pair_u8s8p_t commandKeyArr[] = {
    {CKEYINDEX_GETDEVINFO,    "getDevInfo"},
    {CKEYINDEX_HEARTBEAT,     "heartbeat"},
    {CKEYINDEX_PUTCHAR,       "putChar"},         /** 命令下发！长度不定 **/
    {CKEYINDEX_GETCHAR,       "getChar"},         /** 查询单个服务状态！ 长度不定  **/
    {CKEYINDEX_REPORTSERVICE, "reportService,"},  /** 查询单个服务状态！ 长度不定  **/
    {CKEYINDEX_SCANWIFI,      "scanWifi"},        /** production test ack of scanwifi **/
    {CKEYINDEX_CONNECTWIFI,   "connectWifi"},     /** production test ack of connect action **/
    {CKEYINDEX_PUTWIFISTATUS, "putWifiStatus"},   /** the command description !!! **/
    {CKEYINDEX_GETWIFISTATUS, "getWifiStatus"},   /** the command description !!! **/
    {CKEYINDEX_RESETNET,      "resetNet"},        /** reset net and configure net !!! **/
    {CKEYINDEX_GETSSID,       "getSsid"},         /** netInfo ssid !!! **/
    {CKEYINDEX_GETIP,         "getIp"},           /** netInfo ip !!! **/
    {CKEYINDEX_GETMAC,        "getMac"},          /** netInfo mac !!! **/
    {CKEYINDEX_GETRSSI,       "getRssi"},         /** netInfo rssi !!! **/
    {CKEYINDEX_PUTSYNC,       "putSync"},         /** netInfo mac !!! **/
    {CKEYINDEX_SETCMDINTERVAL,"setCmdInterval"},  /** interval time !!! **/
    {CKEYINDEX_GETCMDINTERVAL,"getCmdInterval"},  /** interval time !!! **/
    // {"\"getDevInfo\"", 0},   /**  **/

#define CTestWIFIkeyIdx (MTABSIZE(commandKeyArr))
};

/**
 * Correspondence between index and string(body)
 **/
const pair_u8s8p_t commandBodyArr[] = {
    {CBODYINDEX_OK,                     "ok"},
    {CBODYINDEX_FAIL,                   "fail"},
    {CBODYINDEX_ERROR,                  "error"},
 // {CBODYINDEX_MOP,                    "mop"},
 // {CBODYINDEX_ROLLER,                 "roller"},
    {CBODYINDEX_CLEANWATERBOXSTATE,     "cleanwaterboxstate"},
 // {CBODYINDEX_PUMP,                   "pump"},
    {CBODYINDEX_BATTERY,                "battery"},
//  {CBODYINDEX_CHARGE,                 "charge"},
    {CBODYINDEX_STATUS,                 "status"},
    {CBODYINDEX_VOICEPROMPT,            "voicePrompt"},
//  {CBODYINDEX_VOICEPROMPT,            "voiceprompt"},
    {CBODYINDEX_COMMONFAULTDETECTION,   "commonFaultDetection"},
    
    {CBODYINDEX_NETINFO,        "netInfo"},
    {CBODYINDEX_UPDATE,         "update"},
    {CBODYINDEX_0,         "0"},
    {CBODYINDEX_1,         "1"},
    {CBODYINDEX_8,         "8"},
};

RetStatus getStringIndexbyString(const pair_u8s8p_t* keyArr, u8 keyArr_len, char* str, u8* str_idx)
{
    int i;
    if ((keyArr == NULL) || (str == NULL) || (str_idx == NULL)) {
        return PINVAL;
    }
    for (i = 0; i < keyArr_len; i++) {
        if (strstr(str, keyArr[i].second) != NULL) {
        // if (strcmp(str, keyArr[i].second) == 0) {  /** parhaps end with another character() **/
            *str_idx = keyArr[i].first;
            return POK;
        }
    }
    return PERROR;
}

/**
 * �쵽ָ����key/len/body, ��Ӧ��ָ������Ϣ; û�ж�Ӧ����Ϣ��(CMSG_NONE), ��Ҫ��һ��ʶ����
 **/
const static Quadruple_keylenbody_t identifyKeyBodyMsg[] = {
    {CKEYINDEX_GETCHAR,       3,  CBODYINDEX_MOP,                   CGETCHAR_MOP},              // protocal changed, give up
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_ROLLER,                CGETCHAR_ROLLER},           // protocal changed, give up
    {CKEYINDEX_GETCHAR,       4,  CBODYINDEX_PUMP,                  CGETCHAR_PUMP},             // protocal changed, give up
    {CKEYINDEX_GETCHAR,       18, CBODYINDEX_CLEANWATERBOXSTATE,    CGETCHAR_CLEANWATERBOXSTATE},
    {CKEYINDEX_GETCHAR,       7,  CBODYINDEX_BATTERY,               CGETCHAR_BATTERY},
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_CHARGE,                CGETCHAR_CHARGE},           // protocal changed, give up    
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_STATUS,                CGETCHAR_STATUS},
    {CKEYINDEX_GETCHAR,       11, CBODYINDEX_VOICEPROMPT,           CGETCHAR_VOICEPROMPT},
    {CKEYINDEX_GETCHAR,       20, CBODYINDEX_COMMONFAULTDETECTION,  CGETCHAR_COMMONFAULTDETECTION},
    
    {CKEYINDEX_GETCHAR,       7,  CBODYINDEX_NETINFO,       CGETCHAR_NETINFO},           // protocal changed, give up    
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_UPDATE,        CGETCHAR_UPDATE},            // protocal changed, give up    

    {CKEYINDEX_GETDEVINFO,    2,  CBODYINDEX_OK,            CGETDEVINFO_RSPOK},
    {CKEYINDEX_GETDEVINFO,    5,  CBODYINDEX_ERROR,         CGETDEVINFO_RSPERROR},
    
    {CKEYINDEX_REPORTSERVICE, 2,  CBODYINDEX_OK,            CREPORT_RSPOK},
    {CKEYINDEX_REPORTSERVICE, 5,  CBODYINDEX_ERROR,         CREPORT_RSPERROR},
        
    {CKEYINDEX_RESETNET,      2,  CBODYINDEX_OK,            CRESETNET_RSPOK},
    {CKEYINDEX_RESETNET,      4,  CBODYINDEX_FAIL,          CRESETNET_RSPFAIL},

    {CKEYINDEX_GETWIFISTATUS, 1,  CBODYINDEX_8,             CCONN_ROUTE},
    {CKEYINDEX_GETWIFISTATUS, 1,  CBODYINDEX_1,             CCONN_CLOUD},
    {CKEYINDEX_GETWIFISTATUS, 1,  CBODYINDEX_0,             CDISCONN_CLOUD},
    
    {CKEYINDEX_SETCMDINTERVAL,2,  CBODYINDEX_OK,            CSETCMDINTERVAL_RSPOK},
    {CKEYINDEX_SETCMDINTERVAL,5,  CBODYINDEX_ERROR,         CSETCMDINTERVAL_RSPERROR},
    
    {CKEYINDEX_GETCMDINTERVAL,1,  0,                        CMSG_NONE},
    {CKEYINDEX_GETCMDINTERVAL,2,  0,                        CMSG_NONE},
    {CKEYINDEX_GETCMDINTERVAL,3,  0,                        CMSG_NONE},

    {CKEYINDEX_HEARTBEAT,     0,  0,                        CHEART_BEAT},
    {CKEYINDEX_GETDEVINFO,    0,  0,                        CGETDEVINFO_REQ},

    {CKEYINDEX_PUTSYNC,       0,  0,                        CPUT_SYNC},
    {CKEYINDEX_PUTCHAR,       0,  0,                        CMSG_NONE /* CPUTCHAR_VOICEPROMPT */ },  // !!!!!!
    // {CKEYINDEX_SCANWIFI,      0,  0,                        CSCAN_WIFI},
    // {CKEYINDEX_CONNECTWIFI,   0,  0,                        CCONN_WIFI},
    // {CKEYINDEX_PUTWIFISTATUS, 0,  0,                        CMSG_NONE},

    {CKEYINDEX_GETSSID,       0,  0,                        CMSG_NONE},
    {CKEYINDEX_GETIP,         0,  0,                        CMSG_NONE},
    {CKEYINDEX_GETMAC,        0,  0,                        CMSG_NONE},
    {CKEYINDEX_GETRSSI,       0,  0,                        CMSG_NONE},

    // {CKEYINDEX_PUTCHAR, 0,  CBODYINDEX_ERROR,   CPUT_CHAR},
};

RetStatus KeyBody2Msg(u8 key_idx, u8 body_len, u8 body_idx, msgType_t* msg)
{
    int i;
    if (body_len == 0) {
        for (i = 0; i < MTABSIZE(identifyKeyBodyMsg); i++) {
            if ((identifyKeyBodyMsg[i].key_idx == key_idx)
                && (identifyKeyBodyMsg[i].body_len == 0)) {
                *msg = identifyKeyBodyMsg[i].msg;
                return POK;
            }
        }
        return PERROR;
    }
    
    for (i = 0; i < MTABSIZE(identifyKeyBodyMsg); i++) {
        if ((identifyKeyBodyMsg[i].key_idx == key_idx)
            && ((identifyKeyBodyMsg[i].body_len == 0) || (identifyKeyBodyMsg[i].body_len == body_len))
            && (identifyKeyBodyMsg[i].body_idx == body_idx)) {
            *msg = identifyKeyBodyMsg[i].msg;
            return POK;
        }
    }
    return PERROR;
}

 const pair_u8objType_t key2objTypeArr[] = {
    {CKEYINDEX_GETSSID,       obj_SSID},         /** netInfo ssid !!! **/
    {CKEYINDEX_GETIP,         obj_IP},           /** netInfo ip !!! **/
    {CKEYINDEX_GETMAC,        obj_MAC},          /** netInfo mac !!! **/
    {CKEYINDEX_GETRSSI,       obj_RSSI},         /** netInfo rssi !!! **/
    {CKEYINDEX_PUTCHAR,       obj_PUTCHAR},         /** netInfo rssi !!! **/
};

RetStatus KeyBody2objType(u8 key_idx, objType_t* objType)
{
    int i;
    for (i = 0; i < MTABSIZE(key2objTypeArr); i++) {
        if ((key2objTypeArr[i].first == key_idx)) {
            *objType = key2objTypeArr[i].second;
            return POK;
        }
    }
   return PERROR;
}

/******************************************************************************************/
/** commonFaultDetection report, then roller/pump/charge no need report **/
u8* ComponentFieldArr[] = {
    // &(g_componentStatus.roller),
    // &(g_componentStatus.pump),
    // &(g_componentStatus.battery),
    // &(g_componentStatus.charge),
    &(g_componentStatus.cleanWater),
    // &(g_componentStatus.status),
    &(g_componentStatus.voicePrompt),
    &(g_componentStatus.commonFaultDetection),
    // &(g_componentStatus.netConnection),     // no need
};
void reportStatusOneByOne(void)
{
    /** execute only one time when connectied cloud **/
    static u8 count = 0;
    if (count < MTABSIZE(ComponentFieldArr)) {
        reportComponentStatus(*(ComponentFieldArr[count]));
        count++;
    }
}
/******************************************************************************************/
/**
 * "key",length,"{body}"
 * if receive key/length/body, then return true!
 **/
objType_t sm_receiveData(char *data)
{
    static smStatus_t s_smStatus = sm_init;
    static u16 s_bodyLen = 0;
    static u8 s_keyIdx = 0;
    static u8 offset = 0;
    u8Data_t u8Data;
    objType_t objType;
    msg_t msg;
    u8 chData = '\0';
    u8 bodyIdx;
    // u8 i;

    if(u8FIFOisEmpty(&g_uart2RxQue) == TRUE) { /** no data !!! **/
        return obj_none;
    }
    //(void)u8FIFO_last(&g_uart2RxQue, &chData);
    if (u8FIFOout2_irq(&g_uart2RxQue, &u8Data) == TRUE) {
        chData = u8Data.u8Val;
    }

    if (s_smStatus == sm_init) {   /** identifing key **/
        if (chData != ',') {
            offset = 0;
            return obj_none;
        }
        if (u8FIFO_get(&g_uart2RxQue, 0, (u8*)data) != TRUE) {
            return obj_none;
        }

        /** identify the command key **/
        if (getStringIndexbyString(commandKeyArr, MTABSIZE(commandKeyArr), data, &s_keyIdx) == POK) {
            s_bodyLen = 0;
            s_smStatus = sm_receiveLenStep2;
            return obj_key;
        } else {
            /** unrecongized **/
            u8FIFOinit_irq(&g_uart2RxQue);  // !!!!!!
            return obj_none;
        }
    } else if (s_smStatus == sm_receiveLenStep2) {    /** identifing length  **/
        if (isdigit(chData)) {   // the valid number
            s_bodyLen = (s_bodyLen * 10) + (chData - '0');
        } else if (chData == ',') {
            #if 1
            digit2ascii(s_bodyLen, data);  // ignore
            #else
            sprintf(data, "%d", s_bodyLen);  // δ֪ԭ�������ʧ��! ���� !!!!!!
            #endif
            s_smStatus = sm_receiveBody;
            /** !!! **/
            offset = u8FIFOlength(&g_uart2RxQue) - u8FIFOlength2(&g_uart2RxQue);
            return obj_len;
        } else if ((chData == ' ') || (chData == '\t')) { // ignore the blank and tab
            return obj_none;
        } else if (chData == '\n') {
            u8FIFOinit_irq(&g_uart2RxQue);
            s_smStatus = sm_init;   // over
            
            #if 1
            /** nobody message identified **/
            if (KeyBody2Msg(s_keyIdx, 0, 0, &(msg.msgType)) != PERROR) {
                msgq_in_irq(&g_msgq, &msg);
            }
            #endif
        }
        return obj_none;
    } else if (s_smStatus == sm_receiveBody) {    /** identifing body **/
        /** offset: start of head, end of len **/
        if (u8FIFO_get(&g_uart2RxQue, offset, (u8*)data) != TRUE) {
            return obj_none;
        }
        if (chData == '\n') {
            ClrTimer_irq(&g_timer[1]);     // !!!!!! fellow the Dm6
            u8FIFOinit_irq(&g_uart2RxQue);
            s_smStatus = sm_init;   // over 
        #if 1
            /** step 1: no need identify the body here **/
            if (KeyBody2objType(s_keyIdx, &objType) == POK) {
                #if 0 // ????????????????????????
                for (int i = 0; i < strlen(data); i++) {
                    u8Data.u8Val = data[i];
                    u8FIFOin_irq(&g_uart2TxQue, &u8Data);
                }
                #endif // ???????????????????????�
                return objType;
            }
            /** step 2: identify the body **/
            if (getStringIndexbyString(commandBodyArr, MTABSIZE(commandBodyArr), data, &bodyIdx) != POK) {
                /**unrecognized the body **/
                return obj_body;
            }
        
            /** step 3: identify the body class A, then notify by msg **/
            if (KeyBody2Msg(s_keyIdx, s_bodyLen, bodyIdx, &(msg.msgType)) == POK) {
                msgq_in_irq(&g_msgq, &msg);
            }

        #endif
            return obj_body;
        }

        if (u8FIFOisFull(&g_uart2RxQue) == TRUE) {
            /** warning...  buf full **/
            #if 0
            u8Data.u8Val = 'F';
            u8FIFOin_irq(&g_uart2TxQue, &u8Data);
            #endif
        }
        if (u8FIFOlength(&g_uart2RxQue) >= (s_bodyLen + offset + 1)      /** strlen(",x,") + the "" end of body + the \n end of body **/) {
            u8FIFOinit_irq(&g_uart2RxQue);
            s_smStatus = sm_init;   // over 
            return obj_body;
        }
    } else if  (s_smStatus == sm_end) {           /** identifing end **/
        s_smStatus = sm_init;
    } else {
        /** !!! **/
    }

    return obj_none;
}

RetStatus doNothing(void)
{
    return PNOSYS;
}

RetStatus reportgetSsid(void)
{
    char buf[] = "getSsid,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

RetStatus reportgetIp(void)
{
    char buf[] = "getIp,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

RetStatus reportgetMac(void)
{
    char buf[] = "getMac,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}

RetStatus reportgetRssi(void)
{
    char buf[] = "getRssi,0\n";
    return reportNobodyInfo(buf, strlen(buf));
}
/*******************************************************************************/
/** find first fch, and last lch **/
char isCoupled(char* str, char hch, char tch) 
{
    char* hp = NULL;
    char* tp = NULL;
    
    if((hp = strchr(str, hch)) == NULL) {   /** Search from the head ==> **/
        return (0);
    }

    if((tp = strrchr(str, tch)) == NULL) {  /** Search from the tail <== **/
        return (0);
    }
    if(tp < hp) {
        return (0);
    }

    return (1);
}

/*******************************************************************************
 * prase json body(JsonParseL0)
 * 
 * example:
 * {key1:value1, key2:value2, ... ,keyn:valuen}
 *******************************************************************************/
unsigned char JsonParseL0(char* jsonstrbuf, kv_t* jsonstcarr)
{
    u8 j_u8,k_u8,i_u8 = 0;
    char *p[CMAX1_COUPLE * 2];
    char *pChar = jsonstrbuf;
    jsonstcarr[0].KVIndex = 0;
    // u8Data_t u8Data;

    /** 1. simily as '{ ... }'! **/
    if((jsonstrbuf[0] != '{') || (jsonstrbuf[strlen(jsonstrbuf) - 1] != '}')) {
        return(0);
    }

    jsonstrbuf[strlen(jsonstrbuf) - 1] = '\0';            /** overwrite the start '}' ! **/
    for(j_u8 = 0; j_u8 < strlen(jsonstrbuf); j_u8++) {    /** overwrite the end '{' ! **/
        jsonstrbuf[j_u8] = jsonstrbuf[j_u8 + 1];
    }
    
    while((p[i_u8]  = strtok(pChar, ":,")) != NULL) {    /** split the string ... **/
        i_u8++;
        pChar = NULL;
    }
 
    for(j_u8 = 0; ((j_u8 < i_u8/2) && (j_u8 < CMAX1_COUPLE)); j_u8++) {
        jsonstcarr[j_u8].KVIndex = i_u8/2 - j_u8;
        strcpy(jsonstcarr[j_u8].key, p[j_u8 * 2]);
        strcpy(jsonstcarr[j_u8].value, p[j_u8 * 2 + 1]);

        jsonstcarr[j_u8].key[strlen(jsonstcarr[j_u8].key) - 1] = '\0';           /** overwrite the tail '"' ? **/
        for(k_u8 = 0; k_u8 < strlen(jsonstcarr[j_u8].key); k_u8++)               /** overwrite the head '"' ? **/
        {
            jsonstcarr[j_u8].key[k_u8] = jsonstcarr[j_u8].key[k_u8 + 1];
        }
    }

    return (1);
}


const pair_u8s8p_t L1KeyArr[] = {
    {CL1KEYINDEX_VOICEPROMPT,    "voicePrompt"},
};

const pair_u8s8p_t L0KeyArr[] = {
    {CL0KEYINDEX_SWITCH,    "switch"},
};

const pair_u8s8p_t L0ValueArr[] = {
    {CL0VALUEINDEX_0,    "0"},
    {CL0VALUEINDEX_1,    "1"},
};

const Quadruple_u8u8u8Msg_t L1body[] = {
    {CL0KEYINDEX_SWITCH, CL0VALUEINDEX_0, CL1KEYINDEX_VOICEPROMPT, CMSG_VOPOFF},
    {CL0KEYINDEX_SWITCH, CL0VALUEINDEX_1, CL1KEYINDEX_VOICEPROMPT, CMSG_VOPON},
};

RetStatus L1KeyBody2Msg(u8 L0key_idx, u8 L0value_idx, u8 L1key_idx, msgType_t* msg)
{
    for (int i = 0; i < MTABSIZE(L1body); i++) {
        if ((L1body[i].L0Key_idx == L0key_idx) &&
            (L1body[i].L0Value_idx == L0value_idx) &&
            (L1body[i].L1Key_idx == L1key_idx)) {
            *msg = L1body[i].msg;
            return POK;
        }
    }
    return PERROR;
}

/********************************
 * prase json body
 *
 * Correspondence between index and string(key)
 *
 * example:
 * {"voiceprompt":{"switch": 1}}
 *
 ********************************/
RetStatus JsonParseL1(char* jsonstrbuf, kv_t* jsonstcarr)
{
//identify L1 key
    char* ptr;
    char* ptr_body;
    u8 L1Key_index;
    u8 L0Key_index;
    u8 L0Value_index;

    /** step1:  {"voicePrompt":{"switch": 1}} ==>  "voicePrompt":{"switch": 1} **/
    strip(jsonstrbuf, '{', '}');
    strim(jsonstrbuf);

    /** step2: split the string! "voicePrompt":{"switch": 1} ==> key:value **/
    if ((ptr = strchr(jsonstrbuf, ':')) == NULL) {
        return PERROR;
    }
    ptr_body = ptr + 1;
    *ptr = '\0';

    /** step3: identify L1-key **/
    if (getStringIndexbyString(L1KeyArr, MTABSIZE(L1KeyArr), jsonstrbuf, &L1Key_index) != POK) {
        return PERROR;
    }

    /** step4: **/
    JsonParseL0(ptr_body, jsonstcarr);
    for (int i = 0; (jsonstcarr[i].KVIndex > 0); i++) {
        if ((getStringIndexbyString(L0KeyArr, MTABSIZE(L0KeyArr), jsonstcarr[i].key, &L0Key_index) == POK) &&
            (getStringIndexbyString(L0ValueArr, MTABSIZE(L0ValueArr), jsonstcarr[i].value, &L0Value_index) == POK)) {
            msg_t msg;
            L1KeyBody2Msg(L0Key_index, L0Value_index, L1Key_index, &(msg.msgType));
            msgq_in_irq(&g_msgq, &msg);
            return POK;
        }
    }

    return (PERROR);
}

