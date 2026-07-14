#include "user_common_config.h"
#include "protocol.h"
#include "base_types.h"
#include "aes.h"
#include "crc.h"
#include "user_peripheral.h"

//static uint8_t          m_aes_key[16]    = "TF@uIA9j#C@lXeGc";
static uint32_t m_aes_key[]={0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C};

static pkt_upload_t m_ble_pkt_upload_handle = NULL;

static uint8_t mACK_FLAG = FALSE;
static uint8_t mCmdState = 0;

uint8_t * get_ack_flag(void)
{
		return &mACK_FLAG;
}

uint8_t *get_cmd_state(void)
{
		return &mCmdState;
}

//命令解析
static void receive_packet_handle(const uint16_t cmd, const uint8_t * p_data, const uint8_t data_len)
{
		extern void System_DisablePeriph(void);
	
		switch(cmd){
			case CMD_CTRL_PWR:
				if(p_data[0] == ON){
					WaterSystem_ACK(ON);
					user_led_set(ON);
					user_pwrCtrl_set(ON);
					mCmdState = ((p_data[0]<1) | p_data[1]);
					log_printf("test open\r\n");
				}else if(p_data[0] == OFF){
					WaterSystem_ACK(OFF);
					user_led_set(OFF);
					user_pwrCtrl_set(OFF);
					mCmdState = ((p_data[0]<1) | p_data[1]);
					log_printf("test close\r\n");
				}
				break;
			case CMD_ACK:
				if(p_data[0] == ON){
					log_printf("open success\r\n");
				}else if(p_data[0] == OFF){
					log_printf("close success\r\n");
				}
				mACK_FLAG = (0X80 | p_data[0]);
				user_led_set(ON);
				delay1ms(5);
				user_led_set(OFF);
				
				System_DisablePeriph();
				
				break;
			
		}
}

//接收数据解析
static void rpkt_analysis(uint8_t * const pData, const uint8_t dataLen)
{
    uint32_t PlainTextBuf[4] = {0}, CipherTextBUf[4] = {0};
    if(pData == NULL || dataLen == 0)
        return;
    memcpy(CipherTextBUf, pData, dataLen);
		stc_aes_cfg_t aes_cfg = {
			.pu32Cipher = CipherTextBUf,//(uint32_t *)pData,
			.pu32Plaintext = PlainTextBuf,
			.pu32Key = (uint32_t *)m_aes_key,
		};
		
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, TRUE);
		
		if(Ok == AES_Decrypt( &aes_cfg)){
				pack_data_t *pack_data = (pack_data_t *)PlainTextBuf;
				receive_packet_handle(pack_data->cmd, pack_data->data, pack_data->len);
		}
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, FALSE);
}

//数据打包上传接口
static uint32_t packet_upload_handle(const uint16_t cmd, const uint8_t * pData, const uint8_t dataLen)
{
    uint32_t PlainTextBuf[4] = {0}, CipherTextBUf[4] = {0};
		
		pack_data_t pack_data = {0};
		pack_data.head = PACK_HEAD;
		pack_data.cmd = cmd;
		memcpy(pack_data.data, pData, dataLen);
		pack_data.crc = CRC16_Get8(pack_data.data, pack_data.len);
		pack_data.len = dataLen;
		memcpy(PlainTextBuf, &pack_data, sizeof(pack_data));
		stc_aes_cfg_t aes_cfg = {
			.pu32Cipher = (uint32_t *)CipherTextBUf,
			.pu32Plaintext = (uint32_t *)PlainTextBuf,
			.pu32Key = (uint32_t *)m_aes_key,
		};
		
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, TRUE);
    if(Ok == AES_Encrypt( &aes_cfg)){
				if (m_ble_pkt_upload_handle != NULL){
						return m_ble_pkt_upload_handle( (uint8_t *)CipherTextBUf, 16);
				}
		}
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAes, FALSE);
		return Error;
}

//配置数据包处理接口句柄
pkt_analysis_t packet_cfg(pkt_upload_t pkt_upload_handle)
{
    if (pkt_upload_handle != NULL)
        m_ble_pkt_upload_handle = pkt_upload_handle;
    return rpkt_analysis;
}

void WaterSystem_Set(uint8_t state, uint8_t isLowPower)
{
		uint8_t data[10] = {0};
		data[0] = state;
		data[1] = isLowPower;		//1:低电，0:正常
		packet_upload_handle( 0x01, data, sizeof(data));
		printf("%s", __func__);
}

void WaterSystem_ACK(uint8_t state)
{
		uint8_t data[10] = {0};
		data[0] = state;
		packet_upload_handle( 0x02, data, sizeof(data));
		printf("%s", __func__);
}







