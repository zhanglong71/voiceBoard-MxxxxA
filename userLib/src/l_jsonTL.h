#ifndef __L_JSONTL_H__
#define __L_JSONTL_H__

#define Mu8FIFO_bufLen(q) (MTABSIZE((q)->buf))

jsonTL_t* getReportCmdbyMode(u8 mode);
jsonTL_t* getGetCharCmdbyMode(u8 mode);

jsonTL_t* getDevInfo(u8 idx);
jsonTL_t* getHeartbeat(void);

jsonTL_t* getService(u8 idx);
// jsonTL_t* getConnectWifi(u8 idx);

void sm_sendData(jsonTL_t* p);
void sm_sendData_once(jsonTL_t* jp);
RetStatus strim(char* str) ;
objType_t sm_receiveData(char *data);

u8 getCommandKeyArrLen(void);
// jsonTL_t* getCommandKey(u8 idx);
const pair_u8s8p_t* getCommandKey(u8 idx);

// u8 getIdxbyMode(u8 mode);

RetStatus reportHeartbeat(void);
RetStatus reportDevInfo(unsigned *arg);
RetStatus reportService(unsigned *arg);
// int reportGetCharCmd(void);
// int reportReportCharCmd(void);
RetStatus reportResetNet(void);
RetStatus reportScanWifi(void);
// int reportConnectWifi(void *arg);
void reportBatteryLevel(u8 arg);
RetStatus reportgetCharNetInfo(NetInfo_t* netInfo);
RetStatus reportAckPutSync(void);
RetStatus reportNobodyInfo(char* data, int len);

int reportComponentStatus(u8 statusIndex);

int getCharAckComponentStatus(u8 statusIndex);

// void reportTest(void);
// RetStatus commandIdx2Message(char index, msgType_t* msg);

RetStatus doNothing(void);
// RetStatus reportCommand(char* comm, int len);
RetStatus reportgetSsid(void);
RetStatus reportgetIp(void);
RetStatus reportgetMac(void);
RetStatus reportgetRssi(void);
void reportStatusOneByOne(void);

unsigned char JsonParseL0(char* jsonstrbuf, kv_t* jsonstcarr);
RetStatus JsonParseL1(char* jsonstrbuf, kv_t* jsonstcarr);

#endif
