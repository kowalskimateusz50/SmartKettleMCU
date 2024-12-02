#include "CommunicationData.h"

int checkInputData(process* ProcessData)
{
	for(int i = 0; i < INPUT_FRAME_LENGTH; i++)
	{
		if(ProcessData->InputFrame[i]=='.')
		{
			return 1;
		}
	}
	return 0;
}

int readInputData(process* ProcessData, inputData* InputData)
{
    //Fame overview
    //RPI -> STM32 "TEMPADJUST:XXX;STARTREQ:X;STOPREQ:X;."





}

int extractTemperatureAdjustment()
{

}
