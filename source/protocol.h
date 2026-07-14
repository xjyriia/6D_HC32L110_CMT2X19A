#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

typedef struct {
	uint8_t head;
	uint8_t cmd;
	uint8_t crc;
	uint8_t len;
	uint8_t data[12];
}pack_data_t;

#define PACK_HEAD    	0XA5

typedef enum{
	CMD_CTRL_PWR = 1,
	CMD_ACK,
}cmd_t;


typedef uint32_t (*pkt_upload_t) ( uint8_t * const pData, const uint8_t dataLen);
typedef void (*pkt_analysis_t) (uint8_t * const pData, const uint8_t dataLen);

uint8_t * get_ack_flag(void);
uint8_t *get_cmd_state(void);
pkt_analysis_t packet_cfg(pkt_upload_t pkt_upload_handle);

void WaterSystem_Set(uint8_t state, uint8_t isLowPower);
void WaterSystem_ACK(uint8_t state);

#endif 

