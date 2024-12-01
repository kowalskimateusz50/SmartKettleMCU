/*
 * CommunicationData.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Mateusz Kowalski
 *
 *  Header file wchich include data exchange structure and functions related to data processing
 */

#ifndef INC_COMMUNICATIONDATA_H_
#define INC_COMMUNICATIONDATA_H_

#include<string.h>
#include<stdio.h>

#define INPUT_FRAME_LENGTH 64
#define OUTPUT_FRAME_LENGTH 64

typedef struct process
{
	char InputFrame[INPUT_FRAME_LENGTH];
	char OutputFrame[OUTPUT_FRAME_LENGTH];
	int  SequenceStep;
}process;

typedef struct inputData
{
	int WatchDog;
	int TemperatureAdjust;
	int StartFlag;
	int StopFlag;

}inputData;

typedef struct outputData
{
	int   WatchDog;
	float ActualTemperature;
	int   Status;
	int   ErrorCode;

}outputData;

int checkInputData(process* ProcessData);

int readInputData(process* ProcessData, inputData* InputData);




#endif /* INC_COMMUNICATIONDATA_H_ */
