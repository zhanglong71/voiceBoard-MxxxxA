#ifndef __CONST_H__
#define __CONST_H__

/*******************************************************************************/

/*******************************************************************************
 *
 * constant define here
 *
 *******************************************************************************/
#define	STACKSIZE	8
#define	QUEUESIZE	8

#define U8FIFOSIZE   128

#define CKVTABSIZE (6)

#define	CMAX1_COUPLE		(CKVTABSIZE)    // the max number of kv couple in one object between master and slave (uart1 communicate)

#define	TRUE	1
#define	FALSE	0

#define TIMER_NUM	4

#define TIMER_0 	0		/** 100Hz定时, 0等于关闭时钟 **/
#define TIMER_UNIT 	(1)		/** 100Hz定时, 0等于关闭时钟 **/
#define TIMER_2UNIT 	(2)		/** 100Hz定时, 0等于关闭时钟 **/
#define TIMER_10MS  (TIMER_UNIT)

#define TIMER_50MS	(TIMER_10MS * 5)
#define TIMER_100MS (TIMER_10MS * 10)

#define TIMER_200MS (TIMER_10MS * 20)
#define TIMER_400MS (TIMER_10MS * 40)
#define TIMER_800MS (TIMER_10MS * 80)
#define TIMER_1SEC  (TIMER_10MS * 100)

#define TIMER_6SEC  (TIMER_1SEC * 6)

/*******************************************************************************/
// #define CCR4_Val (40961)
// #define CCR3_Val (27309)
// #define CCR2_Val (350)  // 17.11KHz
// #define CCR2_Val (700)  // 8.55KHz
#define CCR1_Val (1500)    // 4.16KHz
//#define CCR1_Val (375)    // 16.18KHz
//#define CCR1_Val (350)  //17.11KHz

/*******************************************************************************/
#define	CVOP_VOL0		0xe0
//...
#define	CVOP_VOL7		0xe7
#define	CVOP_VOL15		0xef

#define	CVOP_ALLLOOP		0xf1	//play voice again and again
#define	CVOP_ONELOOP		0xf2	//play one voice again and again
#define	CVOP_STOP		0xFE	//stop playing

/*******************************************************************************/

/*******************************************************************************/
/**  state index for sendto wifi ֻҪ²»Ϡͬ¼´¿ɠ**/
#define CINDEX_UNKNOW                 (0)

#define CINDEX_STANDBY                (1)
#define CINDEX_STANDARD               (2)
#define CINDEX_HIGHPOWER              (3)
#define CINDEX_CLEANING               (4)
#define CINDEX_RINSE                  (5)

#define CINDEX_ROLLEROVERLOAD         (6)
#define CINDEX_ROLLERNORMAL           (7)

#define CINDEX_CLEANWATERNORMAL       (8)
#define CINDEX_CLEANWATERSHORTAGE     (9)

#define CINDEX_PUMPNORMAL             (10)
#define CINDEX_PUMPOVERLOAD           (11)
#define CINDEX_PUMPCURRENTSMALL       (12)

#define CINDEX_BATTERYNORMAL          (13)
#define CINDEX_BATTERYLOW             (14)

#define CINDEX_UNCHARGED              (16)
#define CINDEX_CHARGING               (17)
#define CINDEX_CHARGECOMPLETE         (18)
#define CINDEX_CHARGEFAULT            (19)
#define CINDEX_CHARGEREPAIR           (20)
#define CINDEX_NODEFAULT              (21)

#define CINDEX_CONNECTED              (25)
#define CINDEX_DISCONNECTED           (26)
#define CINDEX_NETINFO                (27)
#define CINDEX_UPDATE                 (28)

#define CINDEX_TANKINPLACE            (31)
#define CINDEX_TANKNOTINPLACE         (32)
#define CINDEX_DORMANCY               (33)
#define CINDEX_SCREENSHUTDOWN         (34)
#define CINDEX_NEUTRAL                (35)
#define CINDEX_CONNECTION             (36)
#define CINDEX_FLUSHING               (37) 
#define CINDEX_FULLSEWAGE             (38)
#define CINDEX_SETUP                  (39)

#define CINDEX_VOICEPROMPT_ON         (40)
#define CINDEX_VOICEPROMPT_OFF        (41)

#define CINDEX_NETCONNECTION_OFF      (42)
#define CINDEX_NETCONNECTION_ON       (43)

// #define CINDEX_COMMONFAULTDETECTION   (49)
#define CINDEX_BATTERY                (50)

#define CINDEX_INVALID (0xff)

/*******************************************************************************/
/**  index for from wifi ֻҪ²»Ϡͬ¼´¿ɠ**/

#if 0
#define CINDEX_GETDEVINFO             (0)
#define CINDEX_HEARTBEAT              (1)  
#define CINDEX_PUTCHAR                (2)
#define CINDEX_GETCHAR                (3)
#define CINDEX_REPORTSERVICE          (4)

#define CINDEX_SCANWIFI               (5)
#define CINDEX_CONNECTWIFI            (6)

#define CINDEX_GETRSSI                (7)
#define CINDEX_PUTWIFISTATUS          (8)
#define CINDEX_GETWIFISTATUS          (9)

#define CINDEX_RESETNET               (10)
#define CINDEX_GETSSID                (11)

#define CINDEX_GETIP                  (12)
#define CINDEX_GETMAC                 (13)
#define CINDEX_PUTSYNC                (14)
#endif

#define CKEYINDEX_NONE                   (0)
#define CKEYINDEX_HEARTBEAT              (1)
#define CKEYINDEX_PUTCHAR                (2)
#define CKEYINDEX_GETCHAR                (3)
#define CKEYINDEX_REPORTSERVICE          (4)

#define CKEYINDEX_SCANWIFI               (5)
#define CKEYINDEX_CONNECTWIFI            (6)

#define CKEYINDEX_GETRSSI                (7)
#define CKEYINDEX_PUTWIFISTATUS          (8)
#define CKEYINDEX_GETWIFISTATUS          (9)

#define CKEYINDEX_RESETNET               (10)
#define CKEYINDEX_GETSSID                (11)

#define CKEYINDEX_GETIP                  (12)
#define CKEYINDEX_GETMAC                 (13)
#define CKEYINDEX_PUTSYNC                (14)
#define CKEYINDEX_GETDEVINFO             (15)
#define CKEYINDEX_SETCMDINTERVAL         (16)
#define CKEYINDEX_GETCMDINTERVAL         (17)

/*** L1 key ***/
#define CL1KEYINDEX_VOICEPROMPT          (20)


/*** L0 key ***/
#define CL0KEYINDEX_SWITCH               (30)

/*** L0 value ***/
#define CL0VALUEINDEX_0             (40)
#define CL0VALUEINDEX_1             (41)

/*******************************************************************************/
#define CBODYINDEX_NONE                   (0)
#define CBODYINDEX_FAIL                   (1)
#define CBODYINDEX_ERROR                  (2)
#define CBODYINDEX_MOP                    (3)
#define CBODYINDEX_ROLLER                 (4)
#define CBODYINDEX_CLEANWATERBOXSTATE     (5)
#define CBODYINDEX_PUMP                   (6)
#define CBODYINDEX_BATTERYSTATUS          (7)
#define CBODYINDEX_BATTERY                (7)
#define CBODYINDEX_CHARGE                 (8)
#define CBODYINDEX_NETINFO                (9)
#define CBODYINDEX_UPDATE                 (10)
#define CBODYINDEX_STATUS                 (11)
#define CBODYINDEX_VOICEPROMPT            (12)
#define CBODYINDEX_COMMONFAULTDETECTION   (13)
#define CBODYINDEX_COMMONFAULT            (13)

#define CBODYINDEX_0                      (20)
#define CBODYINDEX_1                      (21)
#define CBODYINDEX_8                      (22)
#define CBODYINDEX_OK                     (23)

/*******************************************************************************/
#endif
