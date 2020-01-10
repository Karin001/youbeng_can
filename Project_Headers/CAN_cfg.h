//_CAN_CFG_H_
//prevent multiple inclusion
#ifndef _CAN_CFG_H_
#define _CAN_CFG_H_

//define environment
#define KAIFABAN 0
#define YANGJIAN 1

//define XTAL 
#define XTAL_4M 4
#define XTAL_8M 8 

//define the bus clock rate
#define BUS_CLOCK_100M 100
#define BUS_CLOCK_32M 32


//define the CAN speed options (kbps)
#define CAN_125_KBPS  125
#define CAN_250_KBPS  250
#define CAN_500_KBPS  500
#define CAN_800_KBPS  800


//specify your configuration here ...
#define ENV KAIFABAN    // develop or produce
#define CAN_BAUDRATE CAN_125_KBPS //pick the CAN bus speed that will be used
#define BUS_CLOCK BUS_CLOCK_32M //pick the bus speed used
#define XTAL XTAL_4M

//checkSUM checkCount
#define CHECK_SUM 0
#define CHECK_COUNT 0
#endif
