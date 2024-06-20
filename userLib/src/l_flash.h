#ifndef __L_FLASH_H__
#define __L_FLASH_H__

RetStatus userData_update(char* inData);
RetStatus flashPage_update(char* inData);
char* flashPage_get(char* outData);

#endif
