/*
 * phev_can.h
 *
 *  Created on: Dec 18, 2019
 *      Author: Administrator
 */

#ifndef PHEV_CAN_H_
#define PHEV_CAN_H_


//macro
#define IPUMP_ID 0x20
#define MOTOR_INITIAL 0
#define MOTOR_SPEEDCTRL 1
#define MOTOR_SLEEP 2
#define MOTOR_RESERVED 3
#define CHECK_ON 1
#define CHECK_OFF 0

//timing values

#if((CAN_BAUDRATE==CAN_125_KBPS) && (XTAL == XTAL_4M))
	#define VALUE_TSEG10 0x0A
	#define VALUE_TSEG20 0x03
	#define VALUE_PRESCA 0x01
#elif((CAN_BAUDRATE==CAN_250_KBPS) && (XTAL == XTAL_4M))
	#define VALUE_TSEG10 0x0A
	#define VALUE_TSEG20 0x03
	#define VALUE_PRESCA 0x00
#elif((CAN_BAUDRATE==CAN_500_KBPS) && (XTAL == XTAL_4M))
	#define VALUE_TSEG10 0x03
	#define VALUE_TSEG20 0x02
	#define VALUE_PRESCA 0x00
#elif((CAN_BAUDRATE==CAN_800_KBPS) && (XTAL == XTAL_4M))
	#define VALUE_TSEG10 0x01
	#define VALUE_TSEG20 0x01
	#define VALUE_PRESCA 0x00
#elif((CAN_BAUDRATE==CAN_125_KBPS) && (XTAL == XTAL_8M))
	#define VALUE_TSEG10 0x0A
	#define VALUE_TSEG20 0x03
	#define VALUE_PRESCA 0x03
#elif((CAN_BAUDRATE==CAN_250_KBPS) && (XTAL == XTAL_8M))
	#define VALUE_TSEG10 0x0A
	#define VALUE_TSEG20 0x03
	#define VALUE_PRESCA 0x01
#elif((CAN_BAUDRATE==CAN_500_KBPS) && (XTAL == XTAL_8M))
	#define VALUE_TSEG10 0x0A
	#define VALUE_TSEG20 0x03
	#define VALUE_PRESCA 0x00
#elif((CAN_BAUDRATE==CAN_800_KBPS) && (XTAL == XTAL_8M))
	#define VALUE_TSEG10 0x05
	#define VALUE_TSEG20 0x02
	#define VALUE_PRESCA 0x00
#endif

typedef struct _CAN_MSG{
	unsigned int id;
	unsigned char data[8];
	unsigned char len;
}CAN_MSG;


typedef struct _IPUMP {
	unsigned int id; //0x20
	unsigned int IPUMP_MotorSpeed; 
	unsigned char MotorOverTemp; 
	unsigned char ControllerOverTemp;
	unsigned char ControllerUnderVoltage;
	unsigned char ControllerOverVoltage;
	unsigned char ControllerOverCurrent;
	unsigned char ControllerDriveFault;
	unsigned char MotorFault;
	unsigned char PositionSensorFalut;
	unsigned char CANFault;
	unsigned char MCUTempSensorFault;
	unsigned char MotorOverCurrent;
	unsigned char MotorOverSpeed;
	unsigned char SpeedControlBigOffset;
	unsigned char MotorReverse;
	unsigned int IPUMP_MostSpeed;
	unsigned char IPUMP_OperationModel;
	unsigned char ControllerPreOverTemp;
	unsigned char MotorPhaseCurrent;
	unsigned char _inner_MotorPhaseCurrent;
	unsigned char IPUMP_Checksum_id20;
	unsigned char IPUMP_RollingCounter_id20;
} IPUMP;

typedef struct _IPUMP2{
	unsigned char IPUMP_Torque;
	unsigned char IPUMP_BusCurrent;
	unsigned int IPUMP_AngleFB;
	unsigned char IPUMP_Kp;
	unsigned char IPUMP_Temperature;
	unsigned char IPUMP_Voltage;
}IPUMP2;

typedef struct _TCU{
	unsigned int TCU_PumpMotorSpeedReq;
	unsigned char TCU_TransFluidTemp;
	unsigned char TCU_OperationModel;
	unsigned char TCU_RollingCounter_10;
	unsigned char TCU_CRCCheck_10;
}TCU;

typedef struct _frame_ctrl{
	unsigned char checkSwitch;
	void (*_setIPUMPFrame)(IPUMP,CAN_MSG);
	void (*_setMsgCycleTime)(unsigned int);
	void (*_crcCheckSum)(unsigned char data[8]);	
}Frame_ctrl;

/*********global variables*************
 * IPUMP20message :factual TX message in CAN bus for id=0x20 message from IPUMP to TCU
 * IPUMPframe: readable structure of id0x20 message  to be sent to TCU at every CAN TX CYCLE(10ms)
 * TCU_frame: readable structure of id0x10 message received from TCU
 **********************/
 extern CAN_MSG IPUMP20message;  
 extern CAN_MSG message;
 extern IPUMP IPUMPframe;
 extern TCU TCU_frame;


extern void formIPUMP(IPUMP *);
extern void setIPUMPFrame(IPUMP *, CAN_MSG *);
extern void readTCUMessage(TCU *, unsigned char data[8]);
extern void callback_TCU10(TCU *pTCU_frame,IPUMP *pIPUMPframe);
extern void callback_TOF();
extern void CAN_init(void);
extern void CAN_send(CAN_MSG *);
extern void CAN_set_rx_callback(void (*ptr)(CAN_MSG));
extern void TIM0_OVF_set();

#endif /* PHEV_CAN_H_ */
