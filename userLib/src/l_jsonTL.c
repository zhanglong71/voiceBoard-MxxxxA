
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
// #include "main.h"

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
        //return PERROR;
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

#if 0   // !!! 
int reportConnectWifi(void *arg)
{
    (void)arg;
    jsonTL_t* p = getConnectWifi(0);
    sm_sendData(p);
    return (TRUE);
}
#endif

static const reportStatusBody_t reportStatusBodyArr[] = {
    { CINDEX_CONNECTED,           "{\"status\":{\"status\":1}}"},             // on line
    // { CINDEX_UNKNOW,              "{\"mop\":{\"status\":0}}"},             // unknow 未知状态(主机断开及其它)
    { CINDEX_STANDBY,             "{\"mop\":{\"status\":1}}"},             // standby 待机
    { CINDEX_STANDARD,            "{\"mop\":{\"status\":2}}"},                   // standard 标准模式
    { CINDEX_HIGHPOWER,           "{\"mop\":{\"status\":3}}"},                   // highPower强力模式
    // { CINDEX_RINSE,               "{\"mop\":{\"status\":4}}"},                   // rinse 大水冲洗模式
    // { CINDEX_CLEANING,            "{\"mop\":{\"status\":5}}"},                   // cleaning 自清洗模式
    
    { CINDEX_ROLLERNORMAL,        "{\"roller\":{\"status\":1}}"},                // normal 滚筒电机正常
    { CINDEX_ROLLEROVERLOAD,      "{\"roller\":{\"status\":2}}"},                // error 滚筒电机故障
    
    { CINDEX_CLEARWATERNORMAL,    "{\"clearWater\":{\"status\":1}}"},             // clear water normal 清水正常
    { CINDEX_CLEARWATERSHORTAGE,  "{\"clearWater\":{\"status\":2}}"},             // clear water shortage 清水不足
    
    { CINDEX_PUMPNORMAL,          "{\"pump\":{\"status\":1}}"},                  // 水泵正常     pumpNormal
    // { CINDEX_PUMPOVERLOAD,        "{\"pump\":{\"status\":2}}"},                  // 水泵过载     pumpOverload
    // { CINDEX_PUMPCURRENTSMALL,    "{\"pump\":{\"status\":3}}"},                  // 水泵电流过小 pumpCurrentTooSmall
    
    { CINDEX_BATTERYNORMAL,       "{\"batterystatus\":{\"status\":1}}"},      // 电池电压在正常范围
    { CINDEX_BATTERYLOW,          "{\"batterystatus\":{\"status\":2}}"},      // 电池电压过低
    { CINDEX_BATTERYLEVEL,        "{\"batterystatus\":{\"level\":%u}}"},
    
    { CINDEX_UNCHARGED,           "{\"charge\":{\"status\":1}}"},         // 没充电
    { CINDEX_CHARGING,            "{\"charge\":{\"status\":2}}"},         // 正在充电
    { CINDEX_CHARGECOMPLETE,      "{\"charge\":{\"status\":3}}"},         // 充电完成(区别不充电场景)
    { CINDEX_CHARGEFAULT,         "{\"charge\":{\"status\":4}}"},         // 充电故障

    { CINDEX_NETINFO,             "{\"netInfo\":{\"IP\":%s,\"RSSI\":%s,\"SSID\":%s}}"},         // 网络信息
    { CINDEX_UPDATE,              "{\"update\":{\"versoin\":1.0.1,\"introduction\":newest,\"progress\":100,\"bootTime\":60}}"},         // 升级信息
};

void reportBatteryLevel(u8 arg)
{
    jsonTL_t jsonTypeTx;
    char buf[U8FIFOSIZE]; 
    // u8 len = 0;
    // u8Data_t u8Data;
    u8 idx = 0;

    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == CINDEX_BATTERYLEVEL) {
            break;
        }
    }
    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return;
    }

    jsonTypeTx.jHead = "reportChar";
    sprintf(buf, reportStatusBodyArr[idx].body, arg);
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
    
#if 0
    strncpy(g_netInfo.rssi, "-17", MTABSIZE(g_netInfo.rssi));
    strncpy(g_netInfo.ssid, "DIISEA-ssid", MTABSIZE(g_netInfo.ssid));
    strncpy(g_netInfo.ip,   "10.23.45.67", MTABSIZE(g_netInfo.ip));
    strncpy(g_netInfo.mac,  "ab:cd:ef:01:23:45", MTABSIZE(g_netInfo.mac));

    // "{\"netInfo\":{\"IP\":%s,\"RSSI\":%s,\"SSID\":%s}}"
    strcpy(buf, "{\"netInfo\":{\"IP\":");
    strcat(buf, g_netInfo.ip);
    strcat(buf, ",\"RSSI\":");
    strcat(buf, g_netInfo.rssi);
    strcat(buf, ",\"SSID\":");
    strcat(buf, g_netInfo.ssid);
    strcat(buf, "}}");
    
    // sprintf(buf, "IP:%s, RSSI: %s, SSID: %s", netInfo->ip, netInfo->rssi, netInfo->ssid);
    // sprintf(buf, "IP:%s, RSSI: %s, SSID: %s", g_netInfo.ip, g_netInfo.rssi, g_netInfo.ssid);
    // sprintf(buf, "{\"netInfo\":{\"IP\":%s,\"RSSI\":%s,\"SSID\":%s}}", g_netInfo.ip, g_netInfo.rssi, g_netInfo.ssid);
    
    // sprintf(buf, "{\"netInfo\":{\"RSSI\":%s}}", g_netInfo.rssi);      // ������Ч
    // sprintf(buf, "{\"netInfo\":{\"SSID\":%s}}", g_netInfo.ssid);      // ������Ч
    // sprintf(buf, "{\"netInfo\":{\"IP\":%s}}", g_netInfo.ip);             // ��Ч
    // sprintf(buf, "{\"netInfo\":{\"MAC\":%s}}", g_netInfo.mac);

    //strncpy(buf, "IP: 10.23.45.67", MTABSIZE(buf));  // ������Ч
    //strncpy(buf, g_netInfo.ip, MTABSIZE(buf));       // ������Ч
    //strcpy(buf, g_netInfo.ip);                       // ������Ч
    //strcpy(buf, g_netInfo.mac);                        // ������Ч
    
    // sprintf(buf, "IP: %s", "10.23.45.67");       // ������Ч
    // sprintf(buf, "%s", g_netInfo.ip);            // ��Ч
    u8Data_t u8Data;
    for (int i = 0; i < strlen(buf); i++) {
        u8Data.u8Val = buf[i];
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
    }
#else
    for (idx = 0; idx < MTABSIZE(reportStatusBodyArr); idx++) {
        if (reportStatusBodyArr[idx].index == CINDEX_NETINFO) {
            break;
        }
    }
    if (idx >= MTABSIZE(reportStatusBodyArr)) {
        return (PERROR);
    }

    jsonTypeTx.jHead = "getChar";
    //sprintf(buf, reportStatusBodyArr[idx].body, netInfo->ip, netInfo->rssi, netInfo->ssid);
    //sprintf(buf, reportStatusBodyArr[idx].body, "0:0:0:0", "-19", "DIISEA-ssid");
    
    // the complete command like this: "{\"netInfo\":{\"IP\":%s,\"RSSI\":%s,\"SSID\":%s}}"
    strcpy(buf, "{\"netInfo\":{\"IP\":");
    strcat(buf, netInfo->ip);
    strcat(buf, ",\"RSSI\":");
    strcat(buf, netInfo->rssi);
    strcat(buf, ",\"SSID\":");
    strcat(buf, netInfo->ssid);
    strcat(buf, "}}");
    
    jsonTypeTx.jBody = buf;
    jsonTypeTx.jLen = strlen(jsonTypeTx.jBody);

    sm_sendData_once(&jsonTypeTx);
#endif
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
        #if 0
        {
            "getDevInfo", 0,
            "{\"v\":\"1.0.1\","
            "\"dv\":\"1.0.0\","
            "\"prodId\":\"2NPQ\","
            "\"deviceTypeId\":\"19F\","
            "\"manufacturerId\":\"hlp\","
            "\"deviceModel\":\"DM6\","
            "\"deviceTypeNameEn\":\"Scrubber\","
            "\"manufacturerNameEn\":\"DIISEA\","
            "\"networkType\":\"AP\","
            "\"acKey\":\"2B5F3377287C4920506E604B326D5A6479F44A6942B1FE3C86CAD3E3A5F9654D6BC810E9D216466D843A0385A723CC8E\","
            "\"productSeries\":\"DM6\","
            "\"productKey\":\"f2b80c7c77b840e4b7017029baab9bf6\","
            "\"marketName\":\"滴水洗地机DM6\","
            //"\"marketName\":\"DIISEA-DM6\","
            "\"brand\":\"滴水科技\"}"
        },
        {
            "getDevInfo", 0,
            "{\"v\":\"1.0.1\","
            "\"dv\":\"1.0.0\","
            "\"prodId\":\"2NPQ\","
            "\"deviceTypeId\":\"19F\","
            "\"manufacturerId\":\"hlp\","
            "\"deviceModel\":\"DM6\","
            "\"deviceTypeNameEn\":\"Scrubber\","
            "\"manufacturerNameEn\":\"DIISEA\","
            "\"networkType\":\"BLE\","
            "\"acKey\":\"2B5F3377287C4920506E604B326D5A6479F44A6942B1FE3C86CAD3E3A5F9654D6BC810E9D216466D843A0385A723CC8E\","
            "\"productSeries\":\"DM6\","
            "\"productKey\":\"f2b80c7c77b840e4b7017029baab9bf6\","
            "\"marketName\":\"滴水洗地机DM6\","
            //"\"marketName\":\"DIISEA-DM6\","
            "\"brand\":\"滴水科技\"}"
        },

        {
            "getDevInfo", 0,
            "{\"v\":\"1.0.1\","
            "\"dv\":\"1.0.0\","
            "\"prodId\":\"2NPQ\","
            "\"deviceTypeId\":\"19F\","
            "\"manufacturerId\":\"hlp\","
            "\"deviceModel\":\"DM6\","
            "\"deviceTypeNameEn\":\"Scrubber\","
            "\"manufacturerNameEn\":\"DIISEA\","
            "\"networkType\":\"BLE\","
            "\"near\":{"
                "\"type\":1,"
                "\"trp\":-8"
            "},"
            "\"acKey\":\"2B5F3377287C4920506E604B326D5A6479F44A6942B1FE3C86CAD3E3A5F9654D6BC810E9D216466D843A0385A723CC8E\","
            "\"productSeries\":\"DM6\","
            "\"productKey\":\"f2b80c7c77b840e4b7017029baab9bf6\","
            "\"marketName\":\"滴水洗地机DM6\","
            "\"brand\":\"滴水科技\"}"
        }
        #endif
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
            #if 0
                "\"sId\":["
                    "\"status\","
                    "\"charge\","
                    "\"clearWater\","
                    "\"roller\","
                    "\"batterystatus\","
                    "\"mop\","
                    "\"netInfo\","
                    "\"update\""
                    "],"
                "\"sType\":["
                    "\"status\","
                    "\"charge\","
                    "\"clearWater\","
                    "\"roller\","
                    "\"batterystatus\","
                    "\"mop\","
                    "\"netInfo\","
                    "\"update\""
                "]"
            #else
                #if 0
                "\"sId\":["
                    "\"status\","
                    "\"charge\","
                    "\"clearWater\","
                    "\"roller\","
                    "\"batterystatus\","
                    "\"mop\","
                    "\"netInfo\""
                    "],"
                "\"sType\":["
                    "\"status\","
                    "\"charge\","
                    "\"clearWater\","
                    "\"roller\","
                    "\"batterystatus\","
                    "\"mop\","
                    "\"netInfo\""
                "]"
                #else
                "\"sId\":["
                    "\"cleanwaterboxstate\","
                    "\"battery\","
                    "\"status\","
                    "\"commonFaultDetection\","
                    "\"Voiceprompt\""
                    "],"
                "\"sType\":["
                    "\"status\","
                    "\"battery\","
                    "\"status\","
                    "\"fault\","
                    "\"Voiceprompt\""
                "]"
                #endif
            #endif
            "}"
        }
       // {"reportService", 0, ""}
    };

    if (idx >= MTABSIZE(ServiceArr)) {
        return (NULL);
    }
    return (&ServiceArr[idx]);
}

#if 0
jsonTL_t* getConnectWifi(u8 idx)
{
    static jsonTL_t ConnectWifiArr[] = {
        {
            "connectWifi", 0,
            "{"
            "\"ssid\":\"hilink_production_test\","
            "\"password\":\"12345678\""
            "}"
        }
    };
    if (idx >= MTABSIZE(ConnectWifiArr)) {
        return (NULL);
    }

    return (&ConnectWifiArr[idx]);
}
#endif

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
    
    while((sp <= end) && isspace(*sp)) {     /** head blank **/
        sp++;
    }
    while((ep >= sp) && isspace(*ep)) {      /** tail blank **/
        ep--;
    }
    len = ((ep < sp) ? 0:((ep - sp) + 1));  /** valid character length **/
    
    for (int i = 0; i < len; i++) {
        str[i] = sp[i];
    }
    str[len] = '\0';
    return POK;
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
    {CKEYINDEX_PUTSYNC,       "putSync"},          /** netInfo mac !!! **/
    // {"\xa5\x5a\x01\x10\x00\x06\x00\x0A\x00\x02", 0, NULL, NULL},
    // {"\"getDevInfo\"", 0},   /**  **/
    // {"\"heartbeat\"", 0},    /** 下发心跳！长度为0 **/

#define CTestWIFIkeyIdx (MTABSIZE(commandKeyArr))
};

#if 0
const pair_u8s8p_t* getCommandKey(u8 idx)
{ 
    if (idx >= MTABSIZE(commandKeyArr)) {
        return (NULL);
    }
    return (&commandKeyArr[idx]);
}

u8 getCommandKeyArrLen(void)
{
    return MTABSIZE(commandKeyArr);
}
#endif

/**
 * Correspondence between index and string(body)
 **/
const pair_u8s8p_t commandBodyArr[] = {
    {CBODYINDEX_OK,             "ok"},
    {CBODYINDEX_FAIL,           "fail"},
    {CBODYINDEX_ERROR,          "error"},
    {CBODYINDEX_MOP,            "mop"},
    {CBODYINDEX_ROLLER,         "roller"},
    {CBODYINDEX_CLEARWATER,     "clearWater"},
    {CBODYINDEX_PUMP,           "pump"},
    {CBODYINDEX_BATTERYSTATUS,  "batterystatus"},
    {CBODYINDEX_CHARGE,         "charge"},
    {CBODYINDEX_STATUS,         "status"},
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

#if 0
RetStatus commandIdx2Message(char index, msgType_t* msg)
{
    int i = 0;
    pair_u8msgType_t const char2msgType[] = {
        {CKEYINDEX_GETDEVINFO, CGETDEVINFO_REQ},
        {CKEYINDEX_HEARTBEAT, CHEART_BEAT},
        {CKEYINDEX_PUTSYNC, CPUT_SYNC},
    };
    for (i = 0; i < MTABSIZE(char2msgType); i++) {
        if (char2msgType[i].first == index) {
            *msg = char2msgType[i].second;
            return POK;
        }
    }
    return PERROR;
}
#endif

/**
 * �쵽ָ����key/len/body, ��Ӧ��ָ������Ϣ; û�ж�Ӧ����Ϣ��(CMSG_NONE), ��Ҫ��һ������
 **/
const static Quadruple_keylenbody_t identifyKeyBodyMsg[] = {
    {CKEYINDEX_GETCHAR,       3,  CBODYINDEX_MOP,           CGETCHAR_MOP},
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_ROLLER,        CGETCHAR_ROLLER},
    {CKEYINDEX_GETCHAR,       4,  CBODYINDEX_PUMP,          CGETCHAR_PUMP},
    {CKEYINDEX_GETCHAR,       10, CBODYINDEX_CLEARWATER,    CGETCHAR_CLEARWATER},
    {CKEYINDEX_GETCHAR,       13, CBODYINDEX_BATTERYSTATUS, CGETCHAR_BATTERY},
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_CHARGE,        CGETCHAR_CHARGE},
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_STATUS,        CGETCHAR_STATUS},
    {CKEYINDEX_GETCHAR,       7,  CBODYINDEX_NETINFO,       CGETCHAR_NETINFO},
    {CKEYINDEX_GETCHAR,       6,  CBODYINDEX_UPDATE,        CGETCHAR_UPDATE},
    
    {CKEYINDEX_GETDEVINFO,    2,  CBODYINDEX_OK,            CGETDEVINFO_RSPOK},
    {CKEYINDEX_GETDEVINFO,    5,  CBODYINDEX_ERROR,         CGETDEVINFO_RSPERROR},
    
    {CKEYINDEX_REPORTSERVICE, 2,  CBODYINDEX_OK,            CREPORT_RSPOK},
    {CKEYINDEX_REPORTSERVICE, 5,  CBODYINDEX_ERROR,         CREPORT_RSPERROR},
        
    {CKEYINDEX_RESETNET,      2,  CBODYINDEX_OK,            CRESETNET_RSPOK},
    {CKEYINDEX_RESETNET,      4,  CBODYINDEX_FAIL,          CRESETNET_RSPFAIL},

    {CKEYINDEX_GETWIFISTATUS, 1,  CBODYINDEX_8,             CCONN_ROUTE},
    {CKEYINDEX_GETWIFISTATUS, 1,  CBODYINDEX_1,             CCONN_CLOUD},
    {CKEYINDEX_GETWIFISTATUS, 1,  CBODYINDEX_0,             CDISCONN_CLOUD},
    
    {CKEYINDEX_HEARTBEAT,     0,  0,                        CHEART_BEAT},
    {CKEYINDEX_GETDEVINFO,    0,  0,                        CGETDEVINFO_REQ},
 
    {CKEYINDEX_PUTSYNC,       0,  0,                        CPUT_SYNC},
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
            && (identifyKeyBodyMsg[i].body_len == body_len)
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
    // ??????????????????????????????
#if 0 
        u8Data.u8Val = chData;
        u8FIFOin_irq(&g_uart2TxQue, &u8Data);
#endif
    // ??????????????????????????????

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

