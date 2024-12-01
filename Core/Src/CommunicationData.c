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

}
