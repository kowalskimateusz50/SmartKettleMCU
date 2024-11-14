/*
 * hmicom.c
 *
 *  Created on: Nov 10, 2024
 *      Author: Mateusz Kowlalski
 */
#include <CommunicationSequence.h>

void CommunicationSequence(process* ProcessData, inputData* InputData, outputData* OutputData, UART_HandleTypeDef* huart4)
{


	switch	(ProcessData->SequenceStep)
	{
		//Initialization
		case 0:
		{
			//Clear input data buffer
			ClearBuffer(huart4, &ProcessData->InputFrame, 63);

			//Clear output data buffer
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

			//Search for 'rd' in InputFrame
			for (int i = 0; i < 63; i++)
			{
				if ((ProcessData->InputFrame[i] == 'r') && (ProcessData->InputFrame[i + 1] == 'd'))
				{
					//Clear input data buffer
					ClearBuffer(huart4, &ProcessData->InputFrame, 63);
					//Go to next step
					ProcessData->SequenceStep = 200;
				}

			}
				//Clear input frame


			break;
		}

		//case 200: Writing output data
		case 200:
		{
	  		/* Create communication frame */
	  		int MessageLength = sprintf((ProcessData->OutputFrame),"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxTemp:%.3f;.",(OutputData->ActualTemperature));

	  		/* Pass message through UART */
	  		HAL_UART_Transmit_IT(huart4, (uint8_t*)(ProcessData->OutputFrame), (MessageLength));

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
					//Clear input data buffer
					ClearBuffer(huart4, &ProcessData->InputFrame, 63);
					//Go to next step
					ProcessData->SequenceStep = 400;
				}

			}
			break;
		}
		//case 400: Reading data from HMI
		case 400:
		{
			for(int i = 0; i < 64; i++)
			{
				if(ProcessData->InputFrame[i]=='.')
				{
					//Go to beginning step
					HAL_Delay(3000);
					ProcessData->SequenceStep = 0;
				}
			}
			break;
		}

	}
}

void ClearBuffer(UART_HandleTypeDef* huart4, char* data, int dataLength)
{
	//Assisgn Uart RXBuffer pointer to beginning and reset RxCounter
	huart4->pRxBuffPtr = &data;
	huart4->RxXferCount = dataLength;

	//Clear data buffer
	for (int i = 0; i < dataLength; i++)
	{
		*data = '\0';
		data++;
	}


}




