/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "music_note.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//����PWM��������������Ķ�ʱ��
#define NOTE_PWM_TIM         htim10
//PWM�����ͨ��
#define NOTE_PWM_CHANNEL     TIM_CHANNEL_1
//��������ʱ�䶨ʱ��
#define NOTE_CONTINUE_TIM    htim9
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//�������ֲ��ŵ�ȫ�ֱ���
uint8_t Music_Volume;     //����
uint8_t Music_Flag;       //�л����ֵı�־λ
uint16_t BGM_Length;      //BGM�������ֵĳ���
Note_TypeDef *BGM_Current;      //ָ��ǰBGM��ָ��
uint8_t test;     //test

unsigned char MoleMusicVolum;   //����,���鷶Χ1~10,1��10С


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_ADC1_Init();
  MX_SPI3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
	MoleMusicVolum = 6;
	enable_music_player();
  music_player(MoleMusicVolum,SUPER_MARIO_MUSIC);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		play_music_block(6);
		if(test>100)
		{
			HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			test=0;
			
		}
//		play_a_note(294,5);
//		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief ʹ�����ֲ�����
  * @param
  * @note  ʹ�ܲ�����������������
  * @retval None
  */
void enable_music_player(void)
{
  HAL_TIM_Base_Start_IT(&NOTE_CONTINUE_TIM);
  HAL_TIM_PWM_Start(&NOTE_PWM_TIM,NOTE_PWM_CHANNEL);
}

/**
  * @brief ����1�����������������÷�������������
  * @param ������Ƶ�ʣ�������
  * @note  �������鷶Χ1~10��1�����10�����������ˡ�
  *        NOTE_PWM_TIM���������PWM�Ķ�ʱ��
  *        NOTE_PWM_CHANNEL�Ǹö�ʱ�����������PWM��ͨ��
  * @retval None
  */
void play_a_note(uint16_t tone,uint8_t volumeLevel)
{
	uint16_t autoReload;
	if((tone<20)||(tone>20000))//̫����̫�ߵ�Ƶ�ʶ���������
	{
    //�Ƚ�ֵ����Ϊ0������
		__HAL_TIM_SET_COMPARE(&NOTE_PWM_TIM,NOTE_PWM_CHANNEL,0);
		__HAL_TIM_SET_COUNTER(&NOTE_PWM_TIM,0);
	}
	else
	{
    //����Ƶ�ʼ����Զ���װֵ
		autoReload=(BEEP_TIM_CLOCK/tone)-1;
    //�����Զ���װֵ
		__HAL_TIM_SET_AUTORELOAD(&NOTE_PWM_TIM,autoReload);
    //���Զ���װֵ���ƣ��ɱ���С����Ϊ�Ƚ�ֵ
		__HAL_TIM_SET_COMPARE(&NOTE_PWM_TIM,TIM_CHANNEL_1,autoReload>>volumeLevel);
    //�ڲ�ʹ�û��������£�����Ѽ���ֵ����
		__HAL_TIM_SET_COUNTER(&NOTE_PWM_TIM,0);
	}
}

/**
  * @brief �������ķ�ʽ������ֻ�ϻ�
  * @param ����
  * @note  �������鷶Χ1~10��1�����10������������
  * @retval None
  */
void play_music_block(uint8_t volume_level)
{
  //��ֻ�ϻ�����
  const Note_TypeDef TwoTigersMusic[]=
  {
    {0,37},//��һ��Ԫ�ص�ʱ��ֵΪ���鳤�� �������������鳤��С1
    {CM1,40},{CM2,40},{CM3,40},{CM1,40},
    {CM1,40},{CM2,40},{CM3,40},{CM1,40},
    {CM3,40},{CM4,40},{CM5,40},{0,40},
    {CM3,40},{CM4,40},{CM5,40},{0,40},
    {CM5,20},{CM6,20},{CM5,20},{CM4,20},{CM3,40},{CM1,40},
    {CM5,20},{CM6,20},{CM5,20},{CM4,20},{CM3,40},{CM1,40},
    {CM1,40},{CL5,40},{CM1,40},{0,40},
    {CM1,40},{CL5,40},{CM1,40},{0,40},
  };

	int i = 1 ;
  int length = TwoTigersMusic[0].time;
	while(i < length) //i<length��ʾ��ǰ���ֻ�û�в�����
	{
	  //�������������ó�һ������������
    play_a_note(TwoTigersMusic[i].tone,volume_level);
    //���������е�ĳ�������ĳ���ʱ���������ʽ��ʱ
    HAL_Delay(TwoTigersMusic[i].time * 10);
    i++;
	}
}

/**
  * @brief ���ű�������
  * @param ��������������
  * @note  �������鷶Χ1~10��1�����10������������
  *        �˺��������ڱ��Դ�ļ��е���
  * @retval None
  */
static void play_BGM(uint8_t volume_level,Note_TypeDef* BGM)
{
  Music_Flag = MUSIC_BEGIN;    //���ø���BGM��־
  BGM_Length = BGM[0].time;
  BGM_Current = BGM;
  Music_Volume = volume_level;
	//printf("���ű������֡�\n");
}

/**
  * @brief �������ֵĴ�����
  * @param None
  * @note  ����ȫ�ֱ�־λMusic_Flag��״̬���ж��Ƿ���Ҫֹͣ��������
  * @retval None
  */
static void BGM_handler(void)
{
	//��̬����������ִ����Ҳ�����ͷ�
  static uint16_t i = 0;//��������ʾ���׵ĵڼ�������
	static uint16_t passed_time = 0;//��ǰ�����Ѿ������˶��ٱ�10�����ʱ��
	static uint16_t curretnt_note_time = 0; //��ǰ������Ҫ�������ٱ�10�����ʱ��
	passed_time++;//ÿ�ε��ô˺�������˱���+1����˵���Ѿ�����10����
	//�����ǰ������û��������ϣ�����Ҫ���κβ������������༴��
	if(passed_time >= curretnt_note_time) //�����ǰ�����������
	{
		if(Music_Flag == MUSIC_BEGIN)//�����һ�β���BGM�����BGM��������ͷ��ʼ
		{
			i = 1;
			Music_Flag = MUSIC_CONTINUE;
		}

		if(i < BGM_Length) //��ǰBGMδ�����꣬��������һ������
		{
			play_a_note(BGM_Current[i].tone,Music_Volume);
			curretnt_note_time = BGM_Current[i].time; //���õ�ǰ��������ʱ��
			i++;
		}
		else//�������
		{
			i = 1;
			Music_Flag = MUSIC_FINISH;
		}
		passed_time = 0;//ʱ������
	}
}


/**
  * @brief �������ֲ��Żص��������ڶ�ʱ�����жϺ����У�ÿ��10ms����һ��
  * @param
  * @note  ����ȫ�ֱ�־λMusic_Flag��״̬���ж��Ƿ���Ҫֹͣ��������
  * @retval None
  */
void music_callback(void)
{
	if((Music_Flag == MUSIC_BEGIN)||(Music_Flag == MUSIC_CONTINUE))
	 BGM_handler();
 else//���ž�ֹ����
	 play_a_note(0,Music_Volume);
}

/**
  * @brief ���ֲ�����
  * @param ���������ֵ����
  * @note  �������鷶Χ1~10��1�����10������������
  * @retval None
  */
void music_player(uint8_t volume_level , enum MusicList num)
{
	switch (num)
	{
		case CREATE_MUSIC:play_BGM(volume_level,(Note_TypeDef *)CreatMusic);break;
		case MISS_MUSIC:play_BGM(volume_level,(Note_TypeDef *)MissMusic);break;
		case TWO_TIGERS_MUSIC:play_BGM(volume_level,(Note_TypeDef *)TwoTigersMusic);break;
		case PEPPA_PIG_MUSIC:play_BGM(volume_level,(Note_TypeDef *)PeppaPigMusic);break;
		case SUPER_MARIO_MUSIC:play_BGM(volume_level,(Note_TypeDef *)SuperMarioMusic);break;
		case DOU_DI_ZHU_MUSIC:play_BGM(volume_level,(Note_TypeDef *)DouDiZhuMusic);break;
		case POLICE_MUSIC:play_BGM(volume_level,(Note_TypeDef *)PoliceMusic);break;
		case AMBULANCE_MUSIC:play_BGM(volume_level,(Note_TypeDef *)AmbulanceMusic);break;
	}
}

//��ʱ������жϴ�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &NOTE_CONTINUE_TIM)
  {
		test++;
    music_callback();
  }

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
