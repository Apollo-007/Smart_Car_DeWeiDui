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
//用于PWM输出，演奏音符的定时器
#define NOTE_PWM_TIM         htim10
//PWM输出的通道
#define NOTE_PWM_CHANNEL     TIM_CHANNEL_1
//音符持续时间定时器
#define NOTE_CONTINUE_TIM    htim9
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//用于音乐播放的全局变量
uint8_t Music_Volume;     //音量
uint8_t Music_Flag;       //切换音乐的标志位
uint16_t BGM_Length;      //BGM背景音乐的长度
Note_TypeDef *BGM_Current;      //指向当前BGM的指针
uint8_t test;     //test

unsigned char MoleMusicVolum;   //音量,建议范围1~10,1大10小


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
  * @brief 使能音乐播放器
  * @param
  * @note  使能不代表立即播放音乐
  * @retval None
  */
void enable_music_player(void)
{
  HAL_TIM_Base_Start_IT(&NOTE_CONTINUE_TIM);
  HAL_TIM_PWM_Start(&NOTE_PWM_TIM,NOTE_PWM_CHANNEL);
}

/**
  * @brief 播放1个音符，根据音调让蜂鸣器发出声音
  * @param 音调（频率），音量
  * @note  音量建议范围1~10，1是最大，10几乎听不清了。
  *        NOTE_PWM_TIM是用于输出PWM的定时器
  *        NOTE_PWM_CHANNEL是该定时器中用于输出PWM的通道
  * @retval None
  */
void play_a_note(uint16_t tone,uint8_t volumeLevel)
{
	uint16_t autoReload;
	if((tone<20)||(tone>20000))//太低与太高的频率都当做无声
	{
    //比较值设置为0，静音
		__HAL_TIM_SET_COMPARE(&NOTE_PWM_TIM,NOTE_PWM_CHANNEL,0);
		__HAL_TIM_SET_COUNTER(&NOTE_PWM_TIM,0);
	}
	else
	{
    //根据频率计算自动重装值
		autoReload=(BEEP_TIM_CLOCK/tone)-1;
    //设置自动重装值
		__HAL_TIM_SET_AUTORELOAD(&NOTE_PWM_TIM,autoReload);
    //将自动重装值右移，成倍变小，作为比较值
		__HAL_TIM_SET_COMPARE(&NOTE_PWM_TIM,TIM_CHANNEL_1,autoReload>>volumeLevel);
    //在不使用缓冲的情况下，必须把计数值清零
		__HAL_TIM_SET_COUNTER(&NOTE_PWM_TIM,0);
	}
}

/**
  * @brief 以阻塞的方式演奏两只老虎
  * @param 音量
  * @note  音量建议范围1~10，1是最大，10几乎听不清了
  * @retval None
  */
void play_music_block(uint8_t volume_level)
{
  //两只老虎乐谱
  const Note_TypeDef TwoTigersMusic[]=
  {
    {0,37},//第一个元素的时间值为数组长度 音符数量比数组长度小1
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
	while(i < length) //i<length表示当前音乐还没有播放完
	{
	  //从乐谱数组中拿出一个音符并播放
    play_a_note(TwoTigersMusic[i].tone,volume_level);
    //根据乐谱中的某个音符的持续时间进行阻塞式延时
    HAL_Delay(TwoTigersMusic[i].time * 10);
    i++;
	}
}

/**
  * @brief 播放背景音乐
  * @param 音量，乐谱数组
  * @note  音量建议范围1~10，1是最大，10几乎听不清了
  *        此函数无需在别的源文件中调用
  * @retval None
  */
static void play_BGM(uint8_t volume_level,Note_TypeDef* BGM)
{
  Music_Flag = MUSIC_BEGIN;    //设置更换BGM标志
  BGM_Length = BGM[0].time;
  BGM_Current = BGM;
  Music_Volume = volume_level;
	//printf("播放背景音乐。\n");
}

/**
  * @brief 背景音乐的处理函数
  * @param None
  * @note  根据全局标志位Music_Flag的状态，判断是否需要停止播放音乐
  * @retval None
  */
static void BGM_handler(void)
{
	//静态变量，函数执行完也不会释放
  static uint16_t i = 0;//索引，表示乐谱的第几个音符
	static uint16_t passed_time = 0;//当前音符已经播放了多少倍10毫秒的时间
	static uint16_t curretnt_note_time = 0; //当前音符需要持续多少倍10毫秒的时间
	passed_time++;//每次调用此函数，则此变量+1，则说明已经过了10毫秒
	//如果当前的音符没有演奏完毕，则不需要做任何操作，继续演奏即可
	if(passed_time >= curretnt_note_time) //如果当前音符演奏完毕
	{
		if(Music_Flag == MUSIC_BEGIN)//如果第一次播放BGM或更换BGM，索引从头开始
		{
			i = 1;
			Music_Flag = MUSIC_CONTINUE;
		}

		if(i < BGM_Length) //当前BGM未演奏完，则演奏下一个音符
		{
			play_a_note(BGM_Current[i].tone,Music_Volume);
			curretnt_note_time = BGM_Current[i].time; //设置当前音符持续时间
			i++;
		}
		else//演奏结束
		{
			i = 1;
			Music_Flag = MUSIC_FINISH;
		}
		passed_time = 0;//时间重置
	}
}


/**
  * @brief 背景音乐播放回调函数，在定时器的中断函数中，每隔10ms调用一次
  * @param
  * @note  根据全局标志位Music_Flag的状态，判断是否需要停止播放音乐
  * @retval None
  */
void music_callback(void)
{
	if((Music_Flag == MUSIC_BEGIN)||(Music_Flag == MUSIC_CONTINUE))
	 BGM_handler();
 else//播放静止音符
	 play_a_note(0,Music_Volume);
}

/**
  * @brief 音乐播放器
  * @param 音量，音乐的序号
  * @note  音量建议范围1~10，1是最大，10几乎听不清了
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

//定时器溢出中断处理函数
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
