#ifndef __PTYPE_H__
#define __PTYPE_H__

/*******************************************************************************
 *
 * All data type define here
 *
 *******************************************************************************/
 #include <const.h>
 
#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
/*******************************************************************************/
typedef enum {
    PNOSYS = -3,   // function not implement
    PINVAL = -2,   // invalid argument
    PERROR = -1,   // unclassified error
    POK = 0,
    PCONTINUE = 1,
} RetStatus;

typedef	int	 (*pfunc_t)(void* arg);
typedef	void (*void_paction_void_t)(void);
typedef	RetStatus (*RetStatus_pfunc_void_t)(void);

typedef struct {
    pfunc_t func;       /** function **/
    unsigned int  *arg;    /** argument **/
} func_t;

typedef struct {
	unsigned char top;
	func_t func[STACKSIZE]; 
}fstack_t;

typedef enum {
    CMSG_NONE = 0, 
    CMSG_TEST,    /** for test only **/
    CMSG_2TEST,    /** for test only **/
/*************
 * 重新拟定消息命名规则
 * 1. 普通以CMSG_xxx形式命名
 * 2. 以CSYS_xxx的形式命令在sys中处理的消息
 *************/    
    CSYS_INIT,  	//
    CMSG_INIT,  	//初始化动作(进入选择)
 
    CMSG_TMR,

    CMSG_DKEY,
    CMSG_UKEY,
    
    // CUART1_TOUT,
    CMSG_UART1RTOUT,
    CMSG_UART1RX,
    CMSG_UART1TX,
    CMSG_UART2RX,
    CMSG_UART2TX,
    
    //C485_STEP,		/** 动作超时 **/
    CMSG_485STEP,		/** 动作超时 **/
	//C485_OVER,		/** 动作完成 **/
	CMSG_485OVER,		/** 动作完成 **/
	
    CMSG_QUERY,
    CGETDEVINFO_REQ,
    CGETDEVINFO_RSPOK,
    CGETDEVINFO_RSPERROR,
    
    CREPORT_RSPOK,
    CREPORT_RSPERROR,
    // CGET_CHAR,
    CGETCHAR_STATUS,
    CGETCHAR_MOP,
    CGETCHAR_ROLLER,
    CGETCHAR_CLEARWATERBOXSTATE,
    CGETCHAR_PUMP,
    CGETCHAR_BATTERY,
    CGETCHAR_CHARGE,
    CGETCHAR_COMMONFAULTDETECTION,
    // CGETCHAR_COMMONFAULT,
    CGETCHAR_NETINFO,
    CGETCHAR_UPDATE,
    CGETCHAR_VOICEPROMPT,
    
    CPUTCHAR_VOICEPROMPT,
    CPUT_CHAR,
    CPUT_SYNC,
    CHEART_BEAT,
    CWIFI_STATUS,
    CCONN_ROUTE,
    CCONN_CLOUD,
    CDISCONN_CLOUD,
    CSCAN_WIFI,
    
    //CCONN_WIFI,
    CWIFI_TEST,
    
    CCONN_WIFI,
    CCONNWIFI_RSPOK,
    CCONNWIFI_RSPFAIL,
       
    CRESET_NET,
    CRESETNET_RSPOK,
    CRESETNET_RSPFAIL,
    
	CPMT_TOUT,
	CPMT_OVER,
	CMSG_VOPON,		    /** voice prompt on **/
	CMSG_VOPOFF,		/** voice prompt off **/
} msgType_t;

typedef struct msg_s {
	msgType_t msgType;
	u8	msgValue;
}msg_t;

typedef struct msg_queue{
	u8	head;
	u8	tail;
	msg_t msg[QUEUESIZE];
} msgq_t;

typedef struct Timer_s {
    u16 tick_bak;
    u16 tick;
    msgType_t msgType;                      /** the type of msg when timeout **/
    //unsigned int count;                   /** Is it necessary ? **/
} Timer_t; 

/*******************************************************************************/
typedef struct jsonTL_s {
	char *jHead;
    u8 jLen;    /** ָ¶¨³¤¶ȡ ȧ¹ûʇ0£¬¾͊ǲ»ȷ¶¨ **/
    char *jBody;
    // void* arg; /** ʕµ½¶ԓ¦üÁϬӦ¶¯׷ »򠆤˼ **/
} jsonTL_t;

/*******************************************************************************/ 
typedef enum {
    sm_none = 0,
    sm_init,
    sm_idle,
    sm_step,
    sm_key,
    sm_sendBody,
    sm_receiveLen,
    sm_receiveLenStep1,
    sm_receiveLenStep2,
    sm_receiveLenStep3,
    sm_receiveBody,
    sm_send,
    sm_end,
    sm_error
} smStatus_t;

/*******************************************************************************/ 
typedef enum {
    obj_none = 0,
    obj_key,
    obj_len,
    obj_body,
    obj_SSID,
    obj_IP,
    obj_MAC,
    obj_RSSI,
    obj_VOICEPROMPT,
    obj_PUTCHAR,
} objType_t;
/*******************************************************************************/ 

typedef struct {
   u8 first;
   u8 second;
} pair_u8u8_t;

typedef struct {
   u8 first;
   objType_t second;
} pair_u8objType_t;

typedef struct {
   u8  key_idx;
   u8  body_len;
   u8  body_idx;
   msgType_t msg;
} Quadruple_keylenbody_t;

#if 0
typedef struct {
   u8  idx;
   u8* ptr;
   u8  value;
} triplet_u8u8pu8_t;
#endif

typedef struct {
   u8  idx;
   u8  status_idx;
   u8* ptr;
} Triplet_u8u8pu8_t;

typedef struct {
   msgType_t msgType;
   u8  status_idx;
   u8* ptr;
} Triplet_msgTypeu8pu8_t;

typedef struct {
   u8 first;
   char* second;
} pair_u8s8p_t;

typedef struct {
   u8 first;
   msgType_t second;
} pair_u8msgType_t;

typedef struct {
   msgType_t first;
   u8* second;
} pair_msgType2u8ptr_t;

typedef struct {
    u8 L0Key_idx;
    u8 L0Value_idx;
    u8 L1Key_idx;
    msgType_t msg;
} Quadruple_u8u8u8Msg_t;
/*******************************************************************************/ 
typedef struct u8Data_s{
	u8 u8Val;
} u8Data_t;

typedef struct u8FIFO_s {
    u8 in;
    u8 out;
    u8 out2;
    //u8 len;
	  //u8 __pad1;
    u8 buf[U8FIFOSIZE];     /** 16 bytes **/
} u8FIFO_t;

/*******************************************************************************/
typedef struct {
    u8FIFO_t* pfifo;
    // Timer_t* ptimer;
    
    u8 sm_status;
    msgType_t stepMsgType;
    msgType_t overMsgType;
} rs485transX_t;
/*******************************************************************************/
#if 1
typedef struct action_s {
	/*****************
	 * MOT-ON
	 * MOT-OFF
	 * LED-ON
	 * LED-OFF
	 * VOP-ON
	 * VOP-OFF
	 * Delay
	 *****************/
#define CACT_MOTONORM	0xff
#define CACT_MOTOREVE	0xfe
#define CACT_MOTOWAIT	0xfd
  
#define CACT_LEDPMTON	0xfb
#define CACT_LEDPMTOFF	0xfa
#define CACT_LEDPMTWAT	0xf9
  
#define CACT_LEDWRNON	0xf7
#define CACT_LEDWRNOFF	0xf6
#define CACT_LEDWRNWAT	0xf5

#define CACT_RS485	0xf4
#define CACT_VOPON	0xf3
//#define CACT_VOPOFF	0xf2
#define CACT_VOPWAT	0xf1
#define CACT_DELAY	0xf0
  
#define CVOP_PWRON	0xef
#define CVOP_PWROFF 0xee


#define CACT_MISC	0xeb


    // paction_t_0		func;
    u16 actionTime;	/** 持续时间 **/
    u16 actionPara;	/** 动作需要的其它参数，如VPidx索引号 **/
    u8 actionType;	/** 动作类型. 似与前面的参数有重叠之嫌，是一个区别动作的依据 **/
    // u8 __pad1;		/** 字节对齐 **/
} action_t;

typedef struct actionQueue_s {
    u8 head;
    u8 tail;
    u8 flag;
    msgType_t stepMsgType;			/** 阶段事件完成消息 **/
    msgType_t overMsgType;			/** 队列事件全部完成消息 **/
    Timer_t *timer;
        #define ACTIONQUEUESIZE 8
    action_t buf[ACTIONQUEUESIZE];     /** 16 bytes **/
} actionQueue_t;
#endif
/*******************************************************************************/
typedef struct _JsonArr
{
    char jName[10];
    char jValue[10];
    unsigned char jIndexN;
} JsonArr;

typedef struct kv_s
{
    char key[10];
    char value[10];
    unsigned char KVIndex;
} kv_t;

/*******************************************************************************/
typedef enum {
	vopIdx_ConnectNo=1,//δÁ¬½Ӊ豸//¹²Ӄ//
	vopIdx_Disconnect=2,//ɨ±¸ґ¶Ͽª
	vopIdx_Install=3,//½øȫɨփģʽ
	vopIdx_VoiceOpen=4,//¿ªƴӯҴ
	vopIdx_VoiceClose=5,//¹رՓ
	vopIdx_WifiReset=6,//͸§¸´λ³ɹ¦
	vopIdx_WifiConnecting=7,//͸§սԚÁ¬½Ӎ
	vopIdx_WifiOk=8,//͸§Á¬½ӳɹ¦
	vopIdx_CHing=9,//¿ªʼ³䵧
	vopIdx_CHcomplete=10,//³䵧ґͪ³ɍ
	vopIdx_Choff=11,//³䵧֐¶ύ
	vopIdx_standard=12,//½øȫ±ꗼģʽ
	vopIdx_RUNm2=13,//½øȫǿÁ¦ģʽ
	vopIdx_nop2=14,//´󋮳叴ģʽ
	vopIdx_RUNCL=15,//½øȫהǥϴģʽ
	vopIdx_RunclOver=16,//הǥϴґͪ³ɍ
	vopIdx_RUNover=17,//ԋА½ኸ£¬ǫ·Żصחùהǥϴ
	vopIdx_RUNOFF=18,//ԋА½ኸ
	vopIdx_Chlowing=19,//µ灿²»ף£¬ǫ¼°ʱ³䵧
	nop3=20,//µ灿²»ף£¬ǫÁ¢¼´³䵧
	nop4=21,//µ灿²»ף£¬ǫ³䵧º󼌐ø
	vopIdx_CHErr=22,//³䵧Ҭ³££¬ǫ¼첩³䵧Ʒ
	vopIdx_ConnectDragLala=23,//ϴµػúґÁ¬½Ӎ
	vopIdx_CisternOk=24,//ˮϤґ°²װ
	vopIdx_CisternNo=25,//ˮϤґȡ³ö
	vopIdx_CisternTake=26,//ˮϤґȡ³ö£¬½øȫ´󋮳叴ģʽ
	vopIdx_sewageErr=27,//ΛˮϤґº£¬ǫǥÀ펛ˮϤ
	vopIdx_ClearErr=28,//ǫ¼ӈ뇥ˮ
	vopIdx_PumpErr=29,//ˮ±õ绺Ҭ³£
	nop5=30,//ˮ±õ绺δ°²װ
	vopIdx_RollerErr=31,//ǫ¼첩¹öͲ
	vopIdx_ConnectAspiration=32,//μ³¾ƷґÁ¬½Ӎ
	vopIdx_DeviceErr=33,//μ³¾Ʒ¹ʕύ
	vopIdx_ConnectFeather=34,//µ綯ǥ½ˢґÁ¬½Ӎ
	nop6=35,//µ綯ǥ½ˢ¹ʕύ
	nop7=36,//ϴ³µ»úґÁ¬½Ӎ
	nop8=37,//ϴ³µ»ú¹ʕύ
	vopIdx_ConnectSpray=38,//»ŧέƷґÁ¬½Ӎ
	vopIdx_RUN3=39,//½øȫ»ɱ¾úģʽ(¼ӊªģʽ)
	vopIdx_40=40,//»ŧέƷ¹ʕύ

	vopIdx_STOP = 0xFE,					/*/ֹͣϮ*/
}vopIdx_t;

typedef enum {
    SM_RS485_IDLE = 0,
    SM_RS485_START = 1,
    SM_RS485_SEND = 2,
    SM_RS485_OVER = 3,
    SM_RS485_ERROR = 0xF1,
} rs485sm_t;

/*******************************************************************************/
typedef struct ComponentField_s{
    u8 mop;          // the mechine work state
	u8 roller;       // roller fault state
	u8 pump;         // pump fault state
	u8 battery;      // Low voltage or not 
	u8 charge;       // charge state
	u8 clearWater;   // tank state
    u8 status;       // the mechine on/off line      
    u8 voicePrompt;  // voice prompt on/off
    u8 commonFaultDetection;  // the last state of  roller/pump/charging!
    /**
     * the mechine state! 
     * 
     * all the state can be updated by uart1(485 VOP play data)
     * voicePrompt can be updated by uart2(APP with cloud)
     * 
     **/
} ComponentField_t;

typedef struct NetInfo_s{
    char ssid[64];
    char ip[20];
    char mac[20];
    char rssi[8];
    int count;
    int flag;
    /** 
     * flag: updated or not
     * bit 0: ssid
     * bit 1: ip
     * bit 2: mac
     * bit 3: rssi
     **/
} NetInfo_t;

typedef struct reportStatusBody_s{
	u8 index;
	char* body;
} reportStatusBody_t;
/*******************************************************************************/
#endif /** ifndef end **/
