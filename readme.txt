基于 航顺HK32F030xMxxA
2024/4/24 15:05:40 printf打印输出(通过485接口)正常
2024/4/25 10:47:37 sysTick定时器生效; 每秒打印一次验证
2024/4/26 10:18:03 查到芯片资料，uart1不能同时用PA.2/PA.3；也不能同时用PC3、PC4用作uartx串口......也就是不能用pin2pin方案不改电路下实现！如此，那就全新的设计吧！！！
                   uart1能用到的引脚是 ？
2024/5/27 16:22:34 计划uart1用(Tx-pin7~PA3、Rx-pin20~PD6), uart2用(Tx-p1n12~PC5、Rx-pin13~PC6) --- 验证 uart1用(pin7~PA3、pin20~PD6) 收到发正常
2024/5/28 8:36:17  验证 uart2用(pin12~PC5、pin13~PC6) 收到发正常；
                   验证TIM2定时器4KHz正常(PD1翻转发2KHz方波)
                   按老流程实现485收发(似乎流程可以再优化)
2024/5/29 14:46:31 验证语音播放正常(飞线串口收发正常; 485收发方向待确认)
                   原有有与wifi模组通信协议已全部实现(通过demo板与PC串口通信方式验证ok)

2024/6/4 16:41:49 准备添加 putChar,xxx,{"voiceprompt":{"switch": 1}} 的应答及动作

原协议：
getChar,3,mop\n
getChar,4,pump\n
getChar,6,status\n
getChar,6,charge\n
getChar,6,roller\n
getChar,10,clearWater\n
getChar,7,battery\n
getChar,7,netInfo\n
getChar,6,update\n

改为
getChar,6,status\n
getChar,7,battery\n
getChar,18,cleanWaterBoxState\n
getChar,11,voiceprompt\n
getChar,20,commonFaultDetection\n


putChar,28,{"voiceprompt":{"switch":1}}\n
  noACK
  
2024/6/13 9:51:28 需要处理的状态：
  1.语音播放受语音开、关控制(仅此开关语音不受控制)(完成待验证)
  2.运行中开始充电，则进入充电状态，退出运行状态(待机)(验证OK)
  3.连接上以后需要将状态全部上报一遍吗？(后面处理)
2024/6/13 17:03:46
  已验证：a.三种工作状态上报ok; 
          b.电池状态: 未充电(不能测试)、充电(测试ok)、电量不足(自测试OK)、电量正常(自测试ok)、电量百分比(未实现)
          c.清水箱状态：清水不足(测试ok)、清水正常(测试ok)、从清水不足到恢复以没有收到报警超时为依据(需要这样实现吗?)
          d.语音开关: (完成待验证)
          e.故障上报: 水泵故障(当前不支持)、充电故障(自验证ok)、滚筒故障(自验证ok)
2024/6/14 11:00:43 加入看门狗功能(自验证整机功能正常)
2024/6/20 9:38:42 验证flash读写正常; 掉电数据不丢失
2024/6/24 10:56:20 修复wifi复位功能(使用了错误的io读取接口！改过来)
2024/6/24 17:36:31 wifi复位功能接口修改(工作时双击 --> 充电时长按3秒); 各种状态定时上报(似不必要，待优化)
2024/6/27 16:54:09 处理wifi模组发来的网络状态及数据上报(连接到云端时才上报数据，上电后只报一次; 通过与PC串口对接抓包的方式验证ok)
2024/7/9 9:00:54 命令识别数组identifyKeyBodyMsg[]中key/len/body/msg中COMMONFAULTDETECTION一项的长度修正(11->20)
2024/7/10 14:49:09 reportService之后增加指令间隔时长 setCmdInterval；
                   按合作方指导修改 cleanwaterboxstate(原为clearwaterboxstate,另有大小写区别) 命令关键字，相应的宏定义全部改掉; 
                   修改commonFaultDetection命令响应(原设计分散到roller/pump/charge状态中)，修改后将roller/pump/charge错误状态赋给 commonFaultDetection;
2024/7/11 8:39:51 自清洗状态(上报3)完成进入待机状态(上报0)
2024/7/12 19:04:17 修改后的版本：
                   a. 净水箱数据上报，格式修改为：reportChar,47,{"cleanwaterboxstate":{"Cleanwaterboxstate":1}}
                   b. 电量数据定时上报：先向主控查数据，再上报给云端！格式 reportChar,24,{"battery":{"level":89}}