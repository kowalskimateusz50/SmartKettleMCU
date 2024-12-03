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
	extractTemperatureAdjustment(ProcessData, InputData);




}

int extractTemperatureAdjustment(process* ProcessData, inputData* InputData)
{
    //Prefixes definition
	char temp
    char tPrefix[] = "TEMPADJUST:";
    char tEndChar = ';';

    //Get temperature reading
    extractDataFromString(ProcessData, tPrefix, tEndChar);

    //Get temperature reading
    iTemperatureReading = stof(extractDataFromString(tInputData, tPrefix, tEndChar));

    if (iTemperatureReading > 0)
        return 1;
    else
        return 0;


}
