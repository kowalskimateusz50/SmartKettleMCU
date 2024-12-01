/*
 * ComSequence.c
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
			ClearInputBuffer(huart4, ProcessData->InputFrame, INPUT_FRAME_LENGTH);
			ClearOutputBuffer(huart4, ProcessData->OutputFrame, OUTPUT_FRAME_LENGTH);

			/* Clear output data buffer
			for(int i = 0; i < 64; i++) {
				ProcessData->OutputFrame[i] = '\0';
			} */

			ProcessData->SequenceStep = 100;
			break;
		}

		//case 100: Awaiting for "rd" command
		case 100:
		{
			//Waiting for message from ISR
			//Check if there is "rd" command

			//Search for 'rd' in InputFrame

				if (CheckReadRequest(ProcessData))
				{
					//Clear input data buffer
					ClearInputBuffer(huart4, ProcessData->InputFrame, INPUT_FRAME_LENGTH);
					//Go to next step
					ProcessData->SequenceStep = 200;
				}

			break;
		}

		//case 200: Writing output data
		case 200:
		{
	  		/* Create communication frame */
	  		int MessageLength = sprintf((ProcessData->OutputFrame),"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxTemperature:%.3f;.",(OutputData->ActualTemperature));

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
			if (CheckWriteRequest(ProcessData))
			{
					//Clear input data buffer
					ClearInputBuffer(huart4, ProcessData->InputFrame, INPUT_FRAME_LENGTH);
					//Go to next step
					ProcessData->SequenceStep = 400;
			}
			break;
		}
		//case 400: Reading data from HMI
		case 400:
		{
				if(checkInputData(ProcessData))
				{
					//Read input data and save to structures
					readInputData(ProcessData, InputData);

					//End cycle and go to beginning step
					ProcessData->SequenceStep = 0;
				}
			break;
		}

	}
}

void ClearInputBuffer(UART_HandleTypeDef* huart4, char* data, int dataLength)
{
	//Assisgn Uart RXBuffer pointer to beginning and reset RxCounter
	huart4->pRxBuffPtr = (uint8_t*)data;
	huart4->RxXferCount = dataLength - 1;

	//Clear data buffer
	for (int i = 0; i < dataLength; i++)
	{
		*data = '\0';
		data++;
	}
}

void ClearOutputBuffer(UART_HandleTypeDef* huart4, char* data, int dataLength)
{
	//Assisgn Uart TXBuffer pointer to beginning and reset RxCounter
	huart4->pTxBuffPtr = (uint8_t*)data;
	huart4->TxXferCount = dataLength - 1;

	//Clear data buffer
	for (int i = 0; i < dataLength; i++)
	{
		*data = '\0';
		data++;
	}
}

int CheckReadRequest(process* ProcessData)
{
	for (int i = 0; i < INPUT_FRAME_LENGTH - 1; i++)
	{
		if ((ProcessData->InputFrame[i] == 'r') && (ProcessData->InputFrame[i + 1] == 'd'))
		{
			return 1;
		}
	}
	return 0;
}

int CheckWriteRequest(process* ProcessData)
{
	for (int i = 0; i < INPUT_FRAME_LENGTH - 1; i++)
	{
		if ((ProcessData->InputFrame[i] == 'w') && (ProcessData->InputFrame[i + 1] == 'r'))
		{
			return 1;
		}
	}
	return 0;
}


