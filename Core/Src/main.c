/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "onewire.h"
#include "ds18b20.h"
#include "string.h"
#include "LCD_HD44780.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


/*!
 *LCD Display chars arrays
 */
char measure_temperature_message[64];
char adjust_temperature_message[64];

/*!
 *USART communication chars arrays
 */

char HmiInputMessage[10];
char HmiOutputMessage[20];

int DebFlag = 0;

/*!
 *ADC constans
 */
const uint32_t ADC_REG_MAX = 0xfff; // 12-bits
const float ADC_VOLTAGE_MAX = 3.3; // [V]

/*!
 *ADC Conversion results
 */
uint32_t ADC_measurment = 0; //ADC Register value
float ADC_voltage = 0;
uint32_t ADC_voltage_mV = 0;
 //Edge detector
/*!
 *Edge detectors
 */
unsigned char main_edge_detector = 0;
unsigned char usart_edge_detector = 0;
unsigned int count_to_sec = 0;

int edge_count = 0;//Spare variable for debugging purpouse


/*!
 *Temperature adjustment
 */
int temperature_adjust = 0;

int temp_counter_x= 0;


int test_change = 0;

float target_temperature = 0.0;

int temperature_usart_adjust = 0;//temperature usart adjust

float temperature;//Tmperature Measurment

float temperature_offset = 7.0;//Temperature  heating up offset

int usart_temperature = 0;//Sending temperature in serial port



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*!
 *Start button and temperature control button interrupt callback
 */



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){ //interrupt handler
    if(GPIO_Pin == START_BUTTON_Pin){

    	  if(main_edge_detector)
    	  {
    		  main_edge_detector=0;
    	  }
    	  else
    	  {
    		  main_edge_detector=1;
    		  edge_count++;
    	  }

    }
    if(GPIO_Pin == CONTROL_BUTTON_Pin){

     	  if(usart_edge_detector)
     	  {
     		 usart_edge_detector=0;
     	  }
     	  else
     	  {
     		 usart_edge_detector=1;
     		 edge_count++;
     	  }
     }
}

/*!
 *USART Serial port interrupt
 *if serial port is sending something
 *after check value we set temperature_usart_adjust
 */

int MessageIt = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart->Instance == UART4)
  {
  	  HAL_UART_Receive_IT(&huart4,(uint8_t*)HmiInputMessage,11);



  	  /* Check whether listening message emitted */
  	  if(strcmp(HmiInputMessage,"listening"))
  	  {

  		/* Create communication frame */
  		int MessageLength = sprintf(HmiOutputMessage,"Temp:%.3f;",temperature);

  		/* Pass message through UART */
  		HAL_UART_Transmit_IT(&huart4, (uint8_t*)HmiOutputMessage, MessageLength);
  	  }


  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim3);//Timer init in normal mode
  HAL_TIM_Base_Start_IT(&htim2);//Timer init int interruption mode

  LCD_Init();//LCD Init

  DS18B20_Init(DS18B20_Resolution_12bits);//Temperature sensor resolution initialization adjust

  /*!
   *Serial ports initialization
   */
  HAL_StatusTypeDef rx_status = ERROR;
  HAL_UART_Receive_IT(&huart4,(uint8_t*)HmiInputMessage,10);

  HAL_ADC_Start(&hadc1);//Start ADC in blocking mode


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)

  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  //Reading sensor adjustment
	  DS18B20_ReadAll();
	  DS18B20_StartAll();


	  	 /*!
		  *ADC read voltage measurment
		  */
	  	 if(HAL_ADC_PollForConversion(&hadc1,100) == HAL_OK)
	  	 {
	  		 ADC_measurment = HAL_ADC_GetValue(&hadc1);
	  	 }
		//transform ADC_measurment to V and mV
	  	 ADC_voltage = ((float)ADC_measurment / (float)ADC_REG_MAX) * ADC_VOLTAGE_MAX;
	  	 ADC_voltage_mV = (uint32_t)(1000.0*ADC_voltage);

	  	 //Calculating temperature adjust measurment
	  	 if(usart_edge_detector)
	  	 {
	  		temperature_adjust = ADC_voltage_mV/30;
	  	 }
	  	 else
	  	 {
	  		 temperature_adjust = temperature_usart_adjust;
	  	 }

	  	 //Additional temperature adjust value scalling
	  	 if(temperature_adjust > 100)
	  	 {
	  		 temperature_adjust = 100;
	  	 }

	  	 //Calculating temperature adjustment to string variable
	  	 sprintf(adjust_temperature_message,"Temp: %i Oc",temperature_adjust);

	  	 HAL_ADC_Start(&hadc1);

		//save what will be write on LCD
		if(DS18B20_GetTemperature(0, &temperature))
			{
				sprintf(measure_temperature_message,"Temp: %.3f Oc",temperature);
			}


		//display on LCD values of temperature
	    if(main_edge_detector)
	    {
			LCD_Cls();
			//checking what type of temperature will be display - from potentiometer or serial port

			if(usart_edge_detector)
			{
				LCD_Locate(0,0);
				LCD_String("ADJ TEMP POT:");
			}
			else
			{
				LCD_Locate(0,0);
				LCD_String("ADJ TEMP USART:");
			}
			//displaying value of temperature in same line
			LCD_Locate(0,1);
			LCD_String(adjust_temperature_message);

			//Mapping value of temperature to target variable
	    }
	    else
	    {
			//Only reading current temperature state
			LCD_Cls();
			LCD_Locate(0,0);
			LCD_String("Obecna temp:");
			LCD_Locate(0,1);
			LCD_String(measure_temperature_message);
	    }

	  	//Target temperature with float conversion and hysteresis addition
		if(main_edge_detector)
		{
			target_temperature = (float)(temperature_adjust-3.0);
		}

		/*!
		 *Main temperature controlling system logic
		 *
		 *if target temperature is bigger than current temperature turn on water heater and triac cooler
		 *else turn off water heater and triac cooler
		 */
		if((target_temperature > temperature) && main_edge_detector == 0)
		{
			//Turn on heating system
			HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, 1);
			//Turn oN triack cooler fan
			HAL_GPIO_WritePin(TRIAC_COOLING_GPIO_Port, TRIAC_COOLING_Pin, 1);
			//Turn of green led indicator
			HAL_GPIO_WritePin(LDGREEN_GPIO_Port, LDGREEN_Pin, 0);
		}
		else
		{
			//Turn off heating system
			HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, 0);
			//Turn off triack cooler fan
			HAL_GPIO_WritePin(TRIAC_COOLING_GPIO_Port, TRIAC_COOLING_Pin, 0);
			//Turn on green led indicator
			HAL_GPIO_WritePin(LDGREEN_GPIO_Port, LDGREEN_Pin, 1);
		}

		HAL_Delay(10);


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
