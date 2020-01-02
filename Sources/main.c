/***************
 * 
 * 1. put "CAN_cfg.h" "phev_can.h" into Project Headers folder, put "phev_can.c" into Sources folder
 * 2. configure macro of ENV CAN_BAUDRATE BUS_CLOCK in "CAN_cfg.h" 
 * 3. put CAN_init() and TIM0_OVF_set() into initial function, and enable total interrupt
 * 4. write callback function: callback_TCU10,this function will be called when you get a CAN message from TCU
 *                        
 */

//MCU and driver header files
#include <hidef.h>
#include "derivative.h"
#include "phev_can.h"
 


//auxiliary functions
void PLL_init(void);
void GPIO_init(void);
void testFun();

//callback functions
void callback_TCU10(TCU *pTCU_frame,IPUMP *pIPUMPframe);



void main(void) {
#if(ENV == KAIFABAN)
	PLL_init();
	testFun();
#endif
	CAN_init();
	TIM0_OVF_set();
	EnableInterrupts;
	for(;;) {
		__RESET_WATCHDOG();
	}
}







/************************start*******************************
 * void callback_TCU10(TCU *pTCU_frame,IPUMP *pIPUMPframe)
 * 
 * description:
 * this function will be called when you get a CAN message from TCU
 * pTCU_frame contains the readable TCU message  which is loaded from the original CAN message
 * pIPUMPframe will be transmit to TCU after this function,  you can set the signals in pIPUMPframe
 * 	
 * 
 **********************************************************/
void callback_TCU10(TCU *pTCU_frame,IPUMP *pIPUMPframe){
			
		
			//for example
	        //1. read the pTCU_frame to know what TCU want 
	        //2. TCU want you to change operation mode ,so you need to do it 
	        //3. once you have done ,you should notify the TCU by setting the signal "IPUMP_OperationModel" in pIPUMPframe
	        
			//delete the code below and write your code...
			pIPUMPframe->IPUMP_MotorSpeed = pTCU_frame->TCU_PumpMotorSpeedReq;
			if(pTCU_frame->TCU_OperationModel == MOTOR_SLEEP){
				pIPUMPframe->IPUMP_MotorSpeed = 0;
			}
			
			
}
/*************************over*************************************/







void testFun(){
	IPUMPframe.IPUMP_MotorSpeed = 1000;
	IPUMPframe.ControllerDriveFault = 1;
	IPUMPframe.ControllerOverTemp = 1;
	IPUMPframe.MotorOverTemp = 1;
	IPUMPframe.ControllerUnderVoltage= 1;
	IPUMPframe.ControllerOverVoltage= 1;
	IPUMPframe.ControllerOverCurrent= 1;
	IPUMPframe.ControllerDriveFault= 1;
	IPUMPframe.MotorFault= 1;
	IPUMPframe.PositionSensorFalut= 1;
	IPUMPframe.CANFault= 1;
	IPUMPframe.MCUTempSensorFault= 1;
	IPUMPframe.MotorOverCurrent= 1;
	IPUMPframe.MotorOverSpeed= 1;
	IPUMPframe.SpeedControlBigOffset= 1;
	IPUMPframe.MotorReverse= 1;
	IPUMPframe.IPUMP_MostSpeed= 5001;
	IPUMPframe.IPUMP_OperationModel= 5;
	IPUMPframe.ControllerPreOverTemp= 1;
	IPUMPframe.MotorPhaseCurrent= 128;
	IPUMPframe.IPUMP_Checksum_id20= 123;
	IPUMPframe.IPUMP_RollingCounter_id20= 6;
	setIPUMPFrame(&IPUMPframe,&IPUMP20message);
}
//initialises the PLL
void PLL_init(void){
	CPMUCLKS_PLLSEL = 1;				//FBUS = FPLL/2.   FBUS = 32MHz, 
	CPMUREFDIV_REFFRQ = 1;				//Reference clock between 2MHZ and 6MHZ.	
	CPMUREFDIV_REFDIV = 0x0;		    //FREF=4/(0+1) = 4MHZ		
	CPMUSYNR_VCOFRQ = 0x1;             	//FVCO is between 48MHZ and 80MHZ	
	CPMUSYNR_SYNDIV = 0x7;				//FVCO = 2xFREFx(SYNDIV+1)   =   FVCO = 2x4x(7+1) = 64MHZ
	CPMUPOSTDIV_POSTDIV = 0x0;			//FPLL = FVCO/(POSTDIV+1).  FPLL = 64MHZ/(0+1)    FPLL = 64MHz	
	CPMUOSC_OSCE = 1;					//External oscillator enable. 4MHZ.        FREF=FOSC/(REFDIV+1)		
	while(!CPMUIFLG_LOCK){}				// Wait for LOCK.      	
	CPMUIFLG = 0xFF;					// clear CMPMU int flags - not needed but good practice    
}

