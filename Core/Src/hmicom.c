/*
 * hmicom.c
 *
 *  Created on: Nov 10, 2024
 *      Author: Mateusz Kowlalski
 */
#include "hmicom.h"

void CommunicationSequence()
{
	switch	(ComSequence)
	{
		//Initialization
		case 0:
		{
			//Clear communication arrays
			for(int i = 0; i < 20; i++)
			{
				HmiInputMessage[i] = '\0';
				HmiOutputMessage[i] = '\0';
			}

			ComSequence = 100;
			break;
		}

		//Wait for "listetning command"
		//case 100: Handling by ISR
		case 100:
		{
			//Clear buffers and Transmit temperature
			//Waiting for message from ISR
			//Check if there is "listetning" command
			if(strcmp(HmiInputMessage, "listetning") == 0)
			{
				//Go to next step
				ComSequence = 200;
			}
			break;
		}
		case 200:
		{
	  		/* Create communication frame */
	  		int MessageLength = sprintf(HmiOutputMessage,"Temp:%.3f;",HmiTemperature);

	  		/* Pass message through UART */
	  		HAL_UART_Transmit_IT(&huart4, (uint8_t*)HmiOutputMessage, (MessageLength));

	  		//End communication cycle and go to begining step
	  		ComSequence = 0;
			break;
		}

	}
}



