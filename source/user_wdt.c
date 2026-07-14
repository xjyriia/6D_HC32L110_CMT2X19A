#include "user_wdt.h"
#include "pca.h"


static volatile uint32_t u32PcaTestFlag = 0;
static volatile uint16_t u16CcapData[8] = {0};
/*******************************************************************************
 * Pca�жϷ������
 ******************************************************************************/
static void PcaInt(void)
{
    if (TRUE == Pca_GetCntIntFlag())
    {
        Pca_ClearCntIntFlag();
        u32PcaTestFlag |= 0x20;
    }
    if (TRUE == Pca_GetIntFlag(Module0))
    {
        Pca_ClearIntFlag(Module0);
        u32PcaTestFlag |= 0x01;
    }
    if (TRUE == Pca_GetIntFlag(Module1))
    {
        Pca_ClearIntFlag(Module1);
        u32PcaTestFlag |= 0x02;
    }
    if (TRUE == Pca_GetIntFlag(Module2))
    {
        Pca_ClearIntFlag(Module2);
        u32PcaTestFlag |= 0x04;
    }
    if (TRUE == Pca_GetIntFlag(Module3))
    {
        Pca_ClearIntFlag(Module3);
        u32PcaTestFlag |= 0x08;
    }
    if (TRUE == Pca_GetIntFlag(Module4))
    {
        Pca_ClearIntFlag(Module4);
        u32PcaTestFlag |= 0x10;
    }
}

/*******************************************************************************
 * PCA WDT���ܲ���
 ******************************************************************************/
en_result_t PcaWdt_Init(void)
{
    stc_pca_config_t stcConfig;
    stc_pca_capmodconfig_t stcModConfig;
    en_result_t      enResult = Ok;
    uint16_t         u16CntData = 0;
    uint16_t         u16CcapData0 = 0xFFFF;  // 增大比较值，延长看门狗超时
    uint16_t         u16CcapData1 = 0xFFFF;
    
    stcConfig.enCIDL = IdleGoon; 
    stcConfig.enWDTE = PCAWDTEnable;
    stcConfig.enCPS  = PCAPCLKDiv32; 
    
    stcConfig.pfnPcaCb = PcaInt;
    
    stcModConfig.enECOM = ECOMEnable;
    stcModConfig.enCAPP = CAPPDisable;
    stcModConfig.enCAPN = CAPNDisable;
    stcModConfig.enMAT  = MATEnable;
    stcModConfig.enTOG  = TOGDisable;
    stcModConfig.enPWM  = PCAPWMDisable;
    
    //PCAʱ�ӿ���
    Clk_SetPeripheralGate(ClkPeripheralPca, TRUE);

    Pca_Cnt16Set(u16CntData);
    Pca_CapData16Set(Module4, u16CcapData0);
    
    if (Ok != Pca_CapModConfig(Module4, &stcModConfig))
    {
        enResult = Error;
        return enResult;
    }
    if (Ok != Pca_Init(&stcConfig))
    {
        enResult = Error;
        return enResult;
    }
    
    Pca_Run();
    return enResult;
}

void pcaWdt_Feed(void)
{
    uint16_t         u16CcapData0 = 0xFFFF;
    uint16_t         u16CcapData1 = 0xFFFF;
	
    //WDT���޸�Ϊwhile(1),�򲻶ϸ��±Ƚ�ֵ����������;����λ��
		if (0xE000 > Pca_Cnt16Get())
		{
			Pca_CapData16Set(Module4, u16CcapData0);
		}
		else
		{
			Pca_CapData16Set(Module4, u16CcapData1);
		}

}










