 /******************************************************************************
 *
 * Module: Dio
 *
 * File Name: Dio.c
 *
 * Description: Source file for Dio Module.
 *
 * Author: Mohamed Tarek
 ******************************************************************************/

#include "Dio.h"
#include "Dio_Regs.h"

#if (DIO_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Dio Modules */
#if ((DET_AR_MAJOR_VERSION != DIO_AR_RELEASE_MAJOR_VERSION)\
 || (DET_AR_MINOR_VERSION != DIO_AR_RELEASE_MINOR_VERSION)\
 || (DET_AR_PATCH_VERSION != DIO_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Det.h does not match the expected version"
#endif

#endif

STATIC const Dio_ConfigChannel * Dio_PortChannels = NULL_PTR;
STATIC uint8 Dio_Status = DIO_NOT_INITIALIZED;

/************************************************************************************
* Service Name: Dio_Init
* Service ID[hex]: 0x10
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the Dio module.
************************************************************************************/
void Dio_Init(const Dio_ConfigType * ConfigPtr)
{
#if (DIO_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (NULL_PTR == ConfigPtr)
	{
		Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_INIT_SID,
		     DIO_E_PARAM_CONFIG);
	}
	else
#endif
	{
		/*
		 * Set the module state to initialized and point to the PB configuration structure using a global pointer.
		 * This global pointer is global to be used by other functions to read the PB configuration structures
		 */
		Dio_Status       = DIO_INITIALIZED;
		Dio_PortChannels = ConfigPtr->Channels; /* address of the first Channels structure --> Channels[0] */
	}
}

/************************************************************************************
* Service Name: Dio_WriteChannel
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
*                  Level - Value to be written.
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to set a level of a channel.
************************************************************************************/
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
	volatile Dio_PortLevelType * Port_Ptr = NULL_PTR;
	boolean error = FALSE;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
	/* Check if the Driver is initialized before using this function */
	if (DIO_NOT_INITIALIZED == Dio_Status)
	{
		Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
				DIO_WRITE_CHANNEL_SID, DIO_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* Check if the used channel is within the valid range */
	if (DIO_CONFIGURED_CHANNLES <= ChannelId)
	{

		Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
				DIO_WRITE_CHANNEL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
#endif

	/* In-case there are no errors */
	if(FALSE == error)
	{
		/* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
		switch(Dio_PortChannels[ChannelId].Port_Num)
		{
			case DIO_PORTA_INDEX:	Port_Ptr = &PORTA_REG;
		        break;
			case DIO_PORTB_INDEX:	Port_Ptr = &PORTB_REG;
		        break;
			case DIO_PORTC_INDEX:	Port_Ptr = &PORTC_REG;
		        break;
			case DIO_PORTD_INDEX:	Port_Ptr = &PORTD_REG;
		        break;
		}
		if(Level == STD_HIGH)
		{
			/* Write Logic High */
			SET_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
		}
		else if(Level == STD_LOW)
		{
			/* Write Logic Low */
			CLEAR_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
		}
	}
	else
	{
		/* No Action Required */
	}

}

/************************************************************************************
* Service Name: Dio_ReadChannel
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_LevelType
* Description: Function to return the value of the specified DIO channel.
************************************************************************************/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
	volatile const Dio_PortLevelType * Pin_Ptr = NULL_PTR;
	boolean error = FALSE;
	Dio_LevelType result = STD_LOW;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
	/* Check if the Driver is initialized before using this function */
	if (DIO_NOT_INITIALIZED == Dio_Status)
	{
		Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
		DIO_READ_CHANNEL_SID, DIO_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* Check if the used channel is within the valid range */
	if (DIO_CONFIGURED_CHANNLES <= ChannelId)
	{

		Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
		DIO_READ_CHANNEL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
#endif

	/* In-case there are no errors */
	if (FALSE == error)
	{
		/* Point to the correct PIN register according to the Port Id stored in the Port_Num member */
		switch (Dio_PortChannels[ChannelId].Port_Num)
		{
		case DIO_PORTA_INDEX:
			Pin_Ptr = &PINA_REG;
			break;
		case DIO_PORTB_INDEX:
			Pin_Ptr = &PINB_REG;
			break;
		case DIO_PORTC_INDEX:
			Pin_Ptr = &PINC_REG;
			break;
		case DIO_PORTD_INDEX:
			Pin_Ptr = &PIND_REG;
			break;
		}
	}

	/* Check the status of the channel */
	if (BIT_IS_SET(*Pin_Ptr, Dio_PortChannels[ChannelId].Ch_Num))
	{
		result = STD_HIGH;
	}
	else
	{
		result = STD_LOW;
	}

	/* returns STD_HIGH or STD_LOW */
	return result;
}

/************************************************************************************
* Service Name: Dio_GetVersionInfo
* Service ID[hex]: 0x12
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): VersionInfo - Pointer to where to store the version information of this module.
* Return value: None
* Description: Function to get the version information of this module.
************************************************************************************/
#if (DIO_VERSION_INFO_API == STD_ON)
void Dio_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
#if (DIO_DEV_ERROR_DETECT == STD_ON)
	/* Check if input pointer is not Null pointer */
	if(NULL_PTR == versioninfo)
	{
		/* Report to DET  */
		Det_ReportError(DIO_MODULE_ID,
						DIO_INSTANCE_ID,
						DIO_GET_VERSION_INFO_SID,
						DIO_E_PARAM_POINTER);
	}
	else
#endif /* (DIO_DEV_ERROR_DETECT == STD_ON) */
	{
		/* Copy the vendor Id */
		versioninfo->vendorID = (uint16)DIO_VENDOR_ID;
		/* Copy the module Id */
		versioninfo->moduleID = (uint16)DIO_MODULE_ID;
		/* Copy Software Major Version */
		versioninfo->sw_major_version = (uint8)DIO_SW_MAJOR_VERSION;
		/* Copy Software Minor Version */
		versioninfo->sw_minor_version = (uint8)DIO_SW_MINOR_VERSION;
		/* Copy Software Patch Version */
		versioninfo->sw_patch_version = (uint8)DIO_SW_PATCH_VERSION;
	}
}
#endif

/************************************************************************************
* Service Name: Dio_FlipChannel
* Service ID[hex]: 0x11
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_LevelType
* Description: Function to flip the level of a channel and return the level of the channel after flip.
************************************************************************************/
#if (DIO_FLIP_CHANNEL_API == STD_ON)
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
	volatile const Dio_PortLevelType * Pin_Ptr = NULL_PTR;
	volatile Dio_PortLevelType * Port_Ptr = NULL_PTR;
	boolean error = FALSE;
	Dio_LevelType result = STD_LOW;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
	/* Check if the Driver is initialized before using this function */
	if (DIO_NOT_INITIALIZED == Dio_Status)
	{
		Det_ReportError(DIO_MODULE_ID,
						DIO_INSTANCE_ID,
						DIO_FLIP_CHANNEL_SID,
						DIO_E_UNINIT);

		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* Check if the used channel is within the valid range */
	if (DIO_CONFIGURED_CHANNLES <= ChannelId)
	{
		Det_ReportError(DIO_MODULE_ID,
						DIO_INSTANCE_ID,
						DIO_FLIP_CHANNEL_SID,
						DIO_E_PARAM_INVALID_CHANNEL_ID);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
#endif

	/* In-case there are no errors */
	if (FALSE == error)
	{
		/* Point to the correct PIN register according to the Port Id stored in the Port_Num member */
		switch (Dio_PortChannels[ChannelId].Port_Num)
		{
		case DIO_PORTA_INDEX:
			Pin_Ptr = &PINA_REG;
			break;
		case DIO_PORTB_INDEX:
			Pin_Ptr = &PINB_REG;
			break;
		case DIO_PORTC_INDEX:
			Pin_Ptr = &PINC_REG;
			break;
		case DIO_PORTD_INDEX:
			Pin_Ptr = &PIND_REG;
			break;
		}

		/* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
		switch (Dio_PortChannels[ChannelId].Port_Num)
		{
		case DIO_PORTA_INDEX:
			Port_Ptr = &PORTA_REG;
			break;
		case DIO_PORTB_INDEX:
			Port_Ptr = &PORTB_REG;
			break;
		case DIO_PORTC_INDEX:
			Port_Ptr = &PORTC_REG;
			break;
		case DIO_PORTD_INDEX:
			Port_Ptr = &PORTD_REG;
			break;
		}
	}

	/* Check the status of the channel */
	if (BIT_IS_SET(*Pin_Ptr, Dio_PortChannels[ChannelId].Ch_Num))
	{
		result = STD_LOW;
		CLEAR_BIT(*Port_Ptr, Dio_PortChannels[ChannelId].Ch_Num);
	}
	else
	{
		result = STD_HIGH;
		SET_BIT(*Port_Ptr, Dio_PortChannels[ChannelId].Ch_Num);
	}

	/* returns last status of the channel */
	return result;
}
#endif

/************************************************************************************
* Service Name: Dio_ReadPort
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): PortId - ID of DIO Port
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_PortLevelType
* Description: Returns the level of all channels of that port.
************************************************************************************/
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
	boolean portIsConfigured = FALSE;
	Dio_PortType loopIteration = 0;
	Dio_PortLevelType Port_Value = STD_LOW;

#if (STD_ON == DIO_DEV_ERROR_DETECT)
	if (DIO_NOT_INITIALIZED == Dio_Status)
	{
		Det_ReportError(DIO_MODULE_ID,
						DIO_INSTANCE_ID,
						DIO_READ_PORT_SID,
						DIO_E_UNINIT);
	}
	else
	{
		/* No action required */
	}

	if (DIO_CONFIGURED_CHANNLES <= PortId)
	{
		Det_ReportError(DIO_MODULE_ID,
						DIO_INSTANCE_ID,
						DIO_READ_PORT_SID,
						DIO_E_PARAM_INVALID_PORT_ID);
	}
	else
	{
		/* No action required */
	}

	/* Check if PortId is one from configured ports */
	for (loopIteration=0; loopIteration<DIO_CONFIGURED_CHANNLES; loopIteration++)
	{
		/* If PortId exists in configured port of channels */
		if (Dio_PortChannels[loopIteration].Port_Num == PortId)
		{
			portIsConfigured = TRUE;

			/* MISRA's rule violation */
			break;
		}
		else
		{
			/* No action required */
		}
	}

	/* Report error in case of wrong PortId */
	if (FALSE == portIsConfigured)
	{
		Det_ReportError(DIO_MODULE_ID,
						DIO_INSTANCE_ID,
						DIO_READ_PORT_SID,
						DIO_E_PARAM_INVALID_PORT_ID);
	}
	else
#endif
	{
		/* Get the correct PIN register's value according to the PortId argument */
		switch (PortId)
		{
		case DIO_PORTA_INDEX: Port_Value = PINA_REG;
				break;
		case DIO_PORTB_INDEX: Port_Value = PINB_REG;
				break;
		case DIO_PORTC_INDEX: Port_Value = PINC_REG;
				break;
		case DIO_PORTD_INDEX: Port_Value = PIND_REG;
				break;
		default:
				break;
		}
	}

	/* Returns value of the corresponding port */
	return Port_Value;
}
