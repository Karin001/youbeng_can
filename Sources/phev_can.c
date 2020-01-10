#include <hidef.h>
#include "derivative.h"

//driver header file

#include "CAN_cfg.h"
#include "phev_can.h"

CAN_MSG IPUMP20message;  
CAN_MSG message;
IPUMP IPUMPframe;
TCU TCU_frame;

/***************************************************************
 * void readTCUMessage(TCU *TCU_frame, unsigned char data[8])
 * 
 * description:
 * load global variable "TCU_frame" from the  data filed of CAN message(id0x10)
 */
void readTCUMessage(TCU *TCU_frame, unsigned char data[8]){
	(*TCU_frame).TCU_PumpMotorSpeedReq = ((((unsigned int)data[0])<<8) | ((unsigned int)data[1]))>>3;
	(*TCU_frame).TCU_TransFluidTemp = data[2];
	(*TCU_frame).TCU_OperationModel = data[3]>>6;
	(*TCU_frame).TCU_RollingCounter_10 = data[6];
	(*TCU_frame).TCU_CRCCheck_10 = data[7];
			
}
/*****void readTCUMessage****************************************/



/************************************************************************
 * void formIPUMP(IPUMP IPUMP_frame)
 * 
 * description:
 * make it legal to the structure of IPUMP_frame 
 */
void formIPUMP(IPUMP *pIPUMP_frame){             
	(*pIPUMP_frame).id = IPUMP_ID;
	(*pIPUMP_frame).IPUMP_MotorSpeed &= 0x1fff;
	(*pIPUMP_frame).MotorOverTemp &= 0x01;
	(*pIPUMP_frame).MotorOverCurrent &= 0x01;
	(*pIPUMP_frame).ControllerOverTemp &= 0x01;
	(*pIPUMP_frame).ControllerUnderVoltage &= 0x01;
	(*pIPUMP_frame).ControllerOverCurrent &= 0x01;
	(*pIPUMP_frame).MCUTempSensorFault &= 0x01;
	(*pIPUMP_frame).ControllerPreOverTemp &= 0x01;
	(*pIPUMP_frame).PositionSensorFalut &= 0x01;
	(*pIPUMP_frame).ControllerDriveFault &= 0x01;
	(*pIPUMP_frame).ControllerOverCurrent &= 0x01;
	(*pIPUMP_frame).ControllerOverVoltage &= 0x01;
	(*pIPUMP_frame).CANFault &= 0x01;
	(*pIPUMP_frame).MotorReverse &= 0x01;
	(*pIPUMP_frame).SpeedControlBigOffset &= 0x01;
	(*pIPUMP_frame).MotorOverSpeed &= 0x01;
	(*pIPUMP_frame).IPUMP_MostSpeed &= 0x1fff;
	(*pIPUMP_frame).IPUMP_RollingCounter_id20 &= 0x0f;
	(*pIPUMP_frame).IPUMP_OperationModel &= 0x07;
	if((*pIPUMP_frame).MotorPhaseCurrent>127){
		(*pIPUMP_frame)._inner_MotorPhaseCurrent = 255;
	}else{
		(*pIPUMP_frame)._inner_MotorPhaseCurrent = ((*pIPUMP_frame).MotorPhaseCurrent<<1);
	}
	
	(*pIPUMP_frame).IPUMP_Checksum_id20 &= 0xff;
}
/***********************************************************/


/************************************************************************
 * void setIPUMPFrame(IPUMP *pIPUMP_frame, CAN_MSG *pMessage)
 * 
 * description:
 * load  message from IPUMP_frame
 */
void setIPUMPFrame(IPUMP *pIPUMP_frame, CAN_MSG *pMessage){
	formIPUMP(pIPUMP_frame);
	(*pMessage).id = (*pIPUMP_frame).id;
	(*pMessage).data[0] = (unsigned char)(((*pIPUMP_frame).IPUMP_MotorSpeed)>>5);
	(*pMessage).data[1] = ((unsigned char)((*pIPUMP_frame).IPUMP_MotorSpeed<<3))|
			((*pIPUMP_frame).MotorOverTemp<<2)|
			((*pIPUMP_frame).ControllerOverTemp<<1)|
			(*pIPUMP_frame).ControllerUnderVoltage;
	(*pMessage).data[2]	= ((*pIPUMP_frame).ControllerOverVoltage<<7)|
			((*pIPUMP_frame).ControllerOverCurrent<<6)|
			((*pIPUMP_frame).ControllerDriveFault<<5)|
			((*pIPUMP_frame).MotorFault<<4)|
			((*pIPUMP_frame).PositionSensorFalut<<3)|
			((*pIPUMP_frame).ControllerPreOverTemp<<2)|
			((*pIPUMP_frame).MCUTempSensorFault<<1)|
			(*pIPUMP_frame).MotorOverCurrent;
	(*pMessage).data[3] = ((*pIPUMP_frame).MotorOverSpeed<<7)|
			(*pIPUMP_frame).SpeedControlBigOffset<<6 |
			(*pIPUMP_frame).MotorReverse<<5 |
			(*pIPUMP_frame).CANFault<<4 |
			((unsigned char)((*pIPUMP_frame).IPUMP_MostSpeed>>9));
	(*pMessage).data[4] = (unsigned char)((*pIPUMP_frame).IPUMP_MostSpeed>>1);
	(*pMessage).data[5] = (((unsigned char)((*pIPUMP_frame).IPUMP_MostSpeed))<<7)|
			((*pIPUMP_frame)._inner_MotorPhaseCurrent>>1);
	(*pMessage).data[6] = ((*pIPUMP_frame)._inner_MotorPhaseCurrent<<7) |
			((*pIPUMP_frame).IPUMP_OperationModel<<4) |
			(*pIPUMP_frame).IPUMP_RollingCounter_id20;
	(*pMessage).data[7] = (*pIPUMP_frame).IPUMP_Checksum_id20;
	(*pMessage).len = 8;
			
}
/********************over*************************/



/**********************************************
 * void TIM0_OVF_set()
 * 
 * description:
 * set can message cycle time 
 */
void TIM0_OVF_set(){
	
#if(BUS_CLOCK == BUS_CLOCK_32M)
	TIM0PTPSR=0x04;
#elif(BUS_CLOCK == BUS_CLOCK_64M)
	TIM0PTPSR=0x09;
#endif
	TIM0TSCR1_PRNT=1;
	TIM0TSCR2_TOI=1;
	TIM0TSCR1_TEN=1;
}
/****************over*****************************/


/*********************************************************
 * CAN module initial function 
 * CAN send basic function
 * CAN receive interrupt basic function
 ********************************************************/

//global variables

void (*rx_callback)(CAN_MSG);



//initialises CAN communications
void CAN_init(void){

	rx_callback = NULL;

	//initialise MSCAN
	CAN0CTL0 = 0x01;
	while (!(CAN0CTL1_INITAK)){}
	CAN0CTL1_CANE = 1;      /* Enable MSCAN module */
	CAN0CTL1_CLKSRC = 0;    /* Clock source is OSCCLK = 4 MHz */    
	CAN0CTL1_LOOPB = 0;     /* Set to 1 for LoopBack Mode, 0 otherwise */
	CAN0CTL1_LISTEN = 0;    /* Not listen only mode */  
	CAN0CTL0_WUPE = 1;		/* Enable WAKEUP */
	
	//Baud rate = CANCLK/(Pre-scaler * (VALUE_TSEG10+VALUE_TSEG20+3))                                                                           
	CAN0BTR1_TSEG_10 = VALUE_TSEG10;
	CAN0BTR1_TSEG_20 = VALUE_TSEG20;
	CAN0BTR0_BRP = VALUE_PRESCA;
	CAN0BTR0_SJW = 0x03;
	CAN0BTR1_SAMP = 0;
	
	//accept all messages
	CAN0IDAC_IDAM = 0; 		/* Two 32 bit filters */
	CAN0IDMR0 = 0xFF;		/* Accept all incoming ID's */
	CAN0IDMR1 = 0xFF;
	CAN0IDMR2 = 0xFF;
	CAN0IDMR3 = 0xFF;
	CAN0IDMR4 = 0xFF;
	CAN0IDMR5 = 0xFF;
	CAN0IDMR6 = 0xFF;
	CAN0IDMR7 = 0xFF;
	
	//exit initialisation mode
	CAN0CTL0_INITRQ = 0;
	while (CAN0CTL1_INITAK){};
	while(!(CAN0CTL0_SYNCH)){};
	
	//clear flags
	CAN0RFLG_RXF = 1;                                   
	CAN0RIER_RXFIE = 1;
	
	//enable physical layer if available
	#ifdef CP0CR
	CP0CR_CPE = 1;
	CP0CR_SPE = 1;
	#endif
	
	//enable RX interrupt
	CAN0RIER_RXFIE = 1;
}

//sends a CAN message
void CAN_send( CAN_MSG *message){
	
	unsigned char index, bufnum;
	
	//prepare frame
	CAN0TIER = 0x00;		/* disable TX interrupts */
	CAN0TBSEL = CAN0TFLG;	/* select lowest empty buffer */
	bufnum = CAN0TBSEL;
	//load ID
	CAN0TXIDR0 = (message->id&0b11111111000)>>3;
	CAN0TXIDR1 = (message->id&0b00000000111)<<5;
	
	//load data
	for(index=0; index<message->len; index++){
		*(&CAN0TXDSR0 + index) = message->data[index];
	}
	
	//send frame
	CAN0TXDLR = message->len;	/* set data length */
	CAN0TXTBPR = 0;		/* set priority */
	CAN0TFLG = bufnum;	/* start transmission */
}

//assigns a function to be called when a message is received
void CAN_set_rx_callback(void (*ptr)(CAN_MSG)){
	rx_callback = ptr;
	
}

// CAN_PROTOCOL tool function
struct _CANCheckTool{
	unsigned char tcu10RxLocalCnt; 
	void (*pCounterAdd)( unsigned char *cnt);
	unsigned char (*pCalcCounterRx)( unsigned char *cntLocal,unsigned char cntRemote);
	unsigned char (*pCheckCRC)(CAN_MSG *message, unsigned char CRCRemote);
	unsigned char (*pcalcCRC)(unsigned char *data);
	
} CANCheckTool;


void CounterAdd( unsigned char *cnt){
	(*cnt)++;
	(*cnt) %= 0x10;
}
unsigned char calcCounterRx(unsigned char *cntLocal,unsigned char cntRemote){
	CounterAdd(cntLocal);
	if(*cntLocal != cntRemote){
		if(*cntLocal != 0){
			(*cntLocal)--;
		}else{
			*cntLocal = 0x0f;
		}
		NULL; // record DTC
		return 0;
	}
	return 1;
}
unsigned char calcCRC(unsigned char *data){
	unsigned char i;
	unsigned char sum = 0;
	for(i=0;i<7;i++){
		sum+=(message->data[i]);
	}
	sum &= 0xf0;
	sum ^= 0xf0;
	return sum;
}
unsigned char CheckCRC(CAN_MSG *message, unsigned char CRCRemote){
	
	if(CRCRemote!=calcCRC(message->data)){
		NULL;//record DTC
		return 0;
	}
	return 1;
	
}
void initCANCheckTool(){
	CANCheckTool.tcu10RxLocalCnt = 0;
	CANCheckTool.pCalcCounterRx = calcCounterRx;
	CANCheckTool.pCheckCRC = CheckCRC;
	CANCheckTool.pCounterAdd = CounterAdd;
	CANCheckTool.pcalcCRC = calcCRC;
}
//CAN message reception interrupt
void interrupt VectorNumber_Vcan0rx CAN_ISR() {
	//extract CAN frame information
	unsigned char i;
	
	
	//copy ID and DLC
	message.id = CAN0RXIDR0<<3;
	message.id |= CAN0RXIDR1>>5;
	message.len = CAN0RXDLR_DLC;
	//copy DATA
	for(i=0; i<message.len; i++){
		message.data[i] = CAN0RXDSR_ARR[i];
	}


	//execute callback
	
//	if(rx_callback!=NULL){
//		rx_callback(message);
//	}
	switch (message.id){
	case 0x10:	
		readTCUMessage(&TCU_frame, message.data);
#if(!CHECK_SUM)
		if(CANCheckTool.pCheckCRC(&message,TCU_frame.TCU_CRCCheck_10)){
			NULL;
		} else{
			break;
		}
#endif
#if(!CHECK_COUNT)
		if(CANCheckTool.pCalcCounterRx(&(CANCheckTool.tcu10RxLocalCnt),TCU_frame.TCU_RollingCounter_10)){
			NULL;
		}else{
			break;
		}
#endif
		callback_TCU10(&TCU_frame, &IPUMPframe); //CALLBACK
		setIPUMPFrame(&IPUMPframe, &IPUMP20message);
	break;
	case 0x60c:
	break;
	default:
	break;
	}
	//clear interrupt flag
	CAN0RFLG_RXF = 1;
}

void callback_TOF(){
	//add cnt and crc  to tx message
	CAN_send(&IPUMP20message);
}
void interrupt VectorNumber_Vtim0ovf TIM0_OVF(void) {
	
	callback_TOF();
	
	TIM0TFLG2_TOF=1;

}
