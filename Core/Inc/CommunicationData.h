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

typedef struct process
{
	char InputFrame[64];
	char ReadFlag[2];
	char OutputFrame[64];
	char WriteFlag[2];
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

#endif /* INC_COMMUNICATIONDATA_H_ */
