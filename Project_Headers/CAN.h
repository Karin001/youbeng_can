//prevent multiple inclusion
#ifndef _CAN_H_
#define _CAN_H_
typedef struct _CAN_MSG{
	unsigned int id;
	unsigned char data[8];
	unsigned char len;
}CAN_MSG;
//public functions
extern void CAN_init(void);
extern void CAN_send(CAN_MSG);
extern void CAN_set_rx_callback(void (*ptr)(CAN_MSG));

//_CAN_H_
#endif
