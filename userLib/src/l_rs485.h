#ifndef __L_RSA85_H__
#define __L_RSA85_H__

void rs485Init(void);
RetStatus rs485_stor_irq(u8Data_t *u8Data);

void rs485actProcess(void);
void rs485TransXDoing(void);
void rs485actOver(void);

RetStatus reportVersion(void);
void generateVoiceAckVer(char* to, u8* ver);
void generateVoiceAckOk(char* to, u8 seq);
void generateVoiceAckErr(char* to, u8 seq);

#endif
