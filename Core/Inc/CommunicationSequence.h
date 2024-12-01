/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "string.h"
#include  <stdio.h>
#include "CommunicationData.h"

/* \brief      Function responsible for clearing Uart buffer and setting buffer pointer at the beginning
* \param
* huart4        Pointer for UART handler structure.
* data          Pointer for data array
* dataLength	Length of data array
* \throws
*/
void ClearInputBuffer(UART_HandleTypeDef* huart4, char* data, int dataLength);

/// \brief      Seq
/// \param      CommunicationData        The data that will be exchanged to the COM port.
/// \throws

void CommunicationSequence(process* ProcessData, inputData* InputData, outputData* OutputData, UART_HandleTypeDef* huart4);

int CheckWriteRequest(process* ProcessData);

int CheckReadRequest(process* ProcessData);
