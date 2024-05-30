#ifndef __L_SYSPROCESS_H__
#define __L_SYSPROCESS_H__

int sysProcess(void *data);
void deamon_task(void);

void checkAndAckGetCharNetInfo(void);
void checkAndAckGetCharUpdate(void);

void netInfoData_init(void);

RetStatus setStatusByvoiceIdx(u8 idx);
RetStatus reportStatusByvoiceIdx(u8 idx);
RetStatus AckgetCharStatusByMsgType(msgType_t msgType);

#endif
