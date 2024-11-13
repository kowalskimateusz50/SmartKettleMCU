/*
 * hmicom.c
 *
 *  Created on: Nov 10, 2024
 *      Author: Mateusz Kowlalski
 */
#include <CommunicationSequence.h>

void CommunicationSequence(process* ProcessData, inputData* InputData, outputData* OutputData)
{


	switch	(ProcessData->SequenceStep)
	{
		//Initialization
		case 0:
		{
			//Clear communication arrays
			for(int i = 0; i < 64; i++) {
				ProcessData->InputFrame[i] = '\0';
			}
			for(int i = 0; i < 64; i++) {
				ProcessData->OutputFrame[i] = '\0';
			}

			ProcessData->SequenceStep = 100;
			break;
		}

		//case 100: Awaiting for "rd" command
		case 100:
		{
			//Waiting for message from ISR
			//Check if there is "rd" command

			if(strcmp(ProcessData->InputFrame, "rd") == 0)
			{
				//Clear input frame
				for(int i = 0; i < 64; i++) {
					ProcessData->InputFrame[i] = '\0';
				}
				//Go to next step
				ProcessData->SequenceStep = 200;
			}
			break;
		}

		//case 200: Writing output data
		case 200:
		{
	  		/* Create communication frame */
	  		int MessageLength = sprintf((ProcessData->OutputFrame),"Temp:%.3f;",(OutputData->ActualTemperature));

	  		/* Pass message through UART */
	  		HAL_UART_Transmit_IT(&huart4, (uint8_t*)(ProcessData->OutputFrame), (MessageLength));

	  		//End communication cycle and go to begining step
	  		ProcessData->SequenceStep = 300;
			break;
		}

		//case 300: Awaiting for writing flag
		case 300:
		{

			//Search for 'wr' in InputFrame
			for (int i = 0; i < 63; i++)
			{
				if ((ProcessData->InputFrame[i] == 'w') && (ProcessData->InputFrame[i + 1] == 'r'))
				{
					//Go to next step
					ProcessData->SequenceStep = 400;
				}

			}
			break;
		}
		//case 400: Reading data from HMI
		case 400:
		{
			for(int i = 0; i != '\0'; i++)
			{
				if(ProcessData->InputFrame[i]=='.')
				{
					//Go to beginning step
					ProcessData->SequenceStep = 0;
				}
			}
			break;
		}

	}
}




