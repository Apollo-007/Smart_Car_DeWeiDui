/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "oled.h"	//只有一个OLED，所以片选CS管脚默认接地
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
uint32_t Time_to_Stop=60000;//Car will stop 60000ms==60s after Run_Flag_Start.
float Kp = 3.98;
float Ki = 1.4;
float Kd = 0.6;
uint32_t t_ms_stop = 0;
uint16_t t_ms = 0;
uint16_t t_100us = 0;
uint16_t t_Run_Flag = 0,t_Debug_Flag = 0;
uint16_t oled_t_ms=0;
uint16_t t_500ms = 0;
int Timer = 0;			//时间
uint8_t speed = 0;		//当前PWM
short TargetSpeed = 10; //目标速度
volatile uint32_t ADC_Value[100];
uint8_t i;
uint8_t	Run_Flag=0,Run_Flag_Start=0,Debug_Flag=0,Debug_Flag_Start=0;
char buffer1[256] = {0};
char buffer2[256] = {0};
char buffer3[256] = {0};
char buffer4[256] = {0};
float XJ_L=0,XJ_R=0,Regulator1=0,Regulator2=0;
int iButtonCount_UKEY;//i代表int型变量，ButtonCount表示按键计数变量
int iButtonCount_Button1,iButtonCount_Button2,iButtonCount_Button3;
int iButtonFlag_UKEY;//i代表int型变量，ButtonFlag表示重按键标志，1代表重新按键，0为没有重新按键
int iButtonFlag_Button1,iButtonFlag_Button2,iButtonFlag_Button3,OLED_RST_Flag=1,OLED_Refresh=0;
int g_iButtonState_UKEY=0;//g是globle代表全局变量，会在其他地方引用；i代表int型变量，ButtonState表示按键标志，1代表按下，0代表松开
int g_iButtonState_Button1=0,g_iButtonState_Button2=0,g_iButtonState_Button3=0;
//用于音乐播放的全局变量
uint8_t Music_Volume;     //音量
uint8_t Music_Flag;       //切换音乐的标志位
uint8_t Music_Off=0;     	//音乐停止的标志位
uint8_t Cycle_Flag=0;     //单曲循环的标志位
uint8_t Music_List=0;     //音乐列表序号
uint16_t BGM_Length;      //BGM背景音乐的长度
Note_TypeDef *BGM_Current;      //指向当前BGM的指针
unsigned char MoleMusicVolum;   //音量,建议范围1~10,1大10小


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void motor_pwm(uint8_t motor,int16_t pwm);
void motor_1khz(int16_t motor_L,int16_t motor_R);

//加入以下代码,支持printf函数,而不需要选择use MicroLIB   
#if 1
#pragma import(__use_no_semihosting)  
//use assert() with HAL
int _ttywrch(int ch)    
{
    ch=ch;
 return ch;
}
//support stdio.h                 
struct __FILE 
{ 
 int handle; 
}; 
/* FILE is typedef'd in stdio.h */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
 x = x; 
} 
//重定义fputc函数 with HAL
int fputc(int ch, FILE *f)//重定向printf到UART
{
 HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
 return ch;
}
#endif

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
	MoleMusicVolum = 6;						//音量,建议范围1~10,1大10小
	enable_music_player();
	HAL_TIM_Base_Start_IT(&htim11);//开启定时器11中断
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_1);//开启定时器1通道1PWM
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);//开启定时器1通道2PWM
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_3);//开启定时器1通道3PWM
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_4);//开启定时器1通道4PWM 
  HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);//开启定时器3编码器捕获
  HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);//开启定时器5编码器捕获
  HAL_UART_Receive_DMA(&huart2,(uint8_t*)&TargetSpeed,sizeof(TargetSpeed));//开启串口DMA接收，默认DMA为循环模式，千万不要放在循环里！！
  HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_Value,100);
	OLED_Init();
//	sprintf(Kp_txt, "KP:%4.2f", Kp);
//	sprintf(Ki_txt, "Ki:%4.2f", Ki);
//	sprintf(Kd_txt, "Kd:%4.2f", Kd);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
//		HAL_Delay(100);
		for(i = 0,Regulator1 = 0,Regulator2 = 0,XJ_L=0,XJ_R=0;i < 60;)//100
		{
			XJ_L += ADC_Value[i++];
			XJ_R += ADC_Value[i++];
			Regulator1 += ADC_Value[i++];
			Regulator2 += ADC_Value[i++];
		}
		XJ_L /= 15;//25
		XJ_R /= 15;
		Regulator1 /= 15;
		Regulator2 /= 15;
//		sprintf(buff,"V = %f ",Regulator1/4096.0*3.3);/4.2
		OLED_Reset_key();		//定时器中断使能每按下按键触发一次为期6毫秒的复位与初始化
		if(OLED_Refresh>123)//每123ms刷新一次
		{
			sprintf(buffer1,"XJ_L = %4.2f ",XJ_L);
			sprintf(buffer2,"XJ_R = %4.2f ",XJ_R);
			sprintf(buffer3,"R1 = %4.2f ",Regulator1);
			sprintf(buffer4,"R2 = %4.2f ",Regulator2);
			OLED_Show_String(0,0,buffer1,BIG);
			OLED_Show_String(0,2,buffer2,BIG);
			OLED_Show_String(0,4,buffer3,BIG);
			OLED_Show_String(0,6,buffer4,BIG);
//			OLED_Show_String(0,0,"3.14",BIG); 
        //函数中		
			
//			OLED_Show_String(0,0,Kp_txt,BIG);
//			OLED_Show_String(0,3,Ki_txt,BIG);
//			OLED_Show_String(0,6,Kd_txt,BIG);
			OLED_Refresh=0;
		}
		if(Run_Flag_Start)
		{
			if(fabs(XJ_L-XJ_R)>=1600)
			{
				if(XJ_L>XJ_R)
				{
					motor_1khz(-500,800);
				}
				else
				{
					motor_1khz(800,-500);
				}
			}
			else if(fabs(XJ_L-XJ_R)>=1000)
			{
				if(XJ_L>XJ_R)
				{
					motor_1khz(-400,760);
				}
				else
				{
					motor_1khz(760,-400);
				}
			}
			else if(fabs(XJ_L-XJ_R)>=600)
			{
				if(XJ_L>XJ_R)
				{
					motor_1khz(0,600);
				}
				else
				{
					motor_1khz(600,0);
				}
			}
			else if(fabs(XJ_L-XJ_R)>=300)
			{
				if(XJ_L>XJ_R)
				{
					motor_1khz(150,600);
				}
				else
				{
					motor_1khz(600,150);
				}
			}
			else if(fabs(XJ_L-XJ_R)>=200)
			{
				if(XJ_L>XJ_R)
				{
					motor_1khz(300,600);
				}
				else
				{
					motor_1khz(600,300);
				}
			}
			else if(fabs(XJ_L-XJ_R)>=100)
			{
				if(XJ_L>XJ_R)
				{
					motor_1khz(450,600);
				}
				else
				{
					motor_1khz(600,450);
				}
			}
			else if(XJ_L<50&&XJ_R<50)
			{
				motor_1khz(0,0);
			}
			else
			{
				motor_1khz(880,880);
			}
			if(t_ms_stop>Time_to_Stop)
			{
				motor_1khz(0,0);
				
				
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
				
				
				HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
				Run_Flag=!Run_Flag;
				t_Run_Flag=0;
				motor_1khz(0,0);
				t_500ms=0;
				t_ms_stop=0;
			}
		}
		if(Debug_Flag_Start)
		{
			//motor_1khz(-400,400);
		
		}
		//HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		if(g_iButtonState_UKEY == 1)
	  {
		  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			if(Music_Volume==10)
				Music_Volume=1;
			else
				Music_Volume++;
		  g_iButtonState_UKEY=0;
	  }
//	  else
//	  {
//		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
//	  }
	  if(g_iButtonState_Button1 == 1)
	  {
			music_player(MoleMusicVolum,SUPER_MARIO_MUSIC);
		  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			Run_Flag=!Run_Flag;
			t_Run_Flag=0;			
		  g_iButtonState_Button1=0;
			motor_1khz(0,0);
			t_500ms=0;
	  }
//		if(Run_Flag)
//		{
//			if(t_Run_Flag<500)
//				
//			
////				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
//			
//			
//			else if(t_Run_Flag<1000)
//				
//			
////				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//			
//			
//			else if(t_Run_Flag<1500)
//				
//			
////				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
//			
//			
//			else if(t_Run_Flag<2000)
//				
//			
////				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//			
//			
//			else if(t_Run_Flag<2500)
//			{
//				t_ms_stop=0;
//				Run_Flag_Start=1;
//				
//				
////				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
//				
//				
//			}
//			else if(t_Run_Flag<3000)
//				
//			
////				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//			
//			
//			else
//			{
//				t_Run_Flag=65432;
//				//Run_Flag_Start=1;*************************************************************
//			}
//		}
//		else
//		{
//			Run_Flag_Start=0;
//			t_Run_Flag=65432;
//		}
	  if(g_iButtonState_Button2 == 1)
	  {
			if(Music_List==5)
				Music_List=0;
			else
				Music_List++;
			if(Music_List==0)
				music_player(MoleMusicVolum,POLICE_MUSIC);
			else if(Music_List==1)
				music_player(MoleMusicVolum,TWO_TIGERS_MUSIC);
			else if(Music_List==2)
				music_player(MoleMusicVolum,SUPER_MARIO_MUSIC);
			else if(Music_List==3)
				music_player(MoleMusicVolum,DOU_DI_ZHU_MUSIC);
			else if(Music_List==4)
				music_player(MoleMusicVolum,PEPPA_PIG_MUSIC);
			else
				music_player(MoleMusicVolum,AMBULANCE_MUSIC);
		  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
			Debug_Flag=!Debug_Flag;
			t_Debug_Flag=0;	
		  g_iButtonState_Button2=0;
			motor_1khz(0,0);
			t_500ms=0;
		  
//		  printf("\r\n********ADC-DMA-Example********\r\n");
//		  printf("XJ_L_Value=%1.3fV\r\n",XJ_L*3.3f/4096);
//		  printf("XJ_R_Value=%1.3fV\r\n",XJ_R*3.3f/4096);
//		  printf("R1_Value=%1.3fV\r\n",Regulator1*3.3f/4096);
//		  printf("R2_Value=%1.3fV\r\n",Regulator2*3.3f/4096);
	  }
//		if(Debug_Flag)
//		{
//			if(t_Debug_Flag<500)
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
//			else if(t_Debug_Flag<1000)
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//			else if(t_Debug_Flag<1500)
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
//			else if(t_Debug_Flag<2000)
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//			else if(t_Debug_Flag<2500)
//			{
//				Debug_Flag_Start=1;
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
//			}
//			else if(t_Debug_Flag<3000)
//				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//			else
//			{
//				t_Debug_Flag=65432;
//				Debug_Flag_Start=1;//*************************************************************
//			}
//		}
//		else
//		{
//			Debug_Flag_Start=0;
//			t_Debug_Flag=65432;
//		}
	  if(g_iButtonState_Button3 == 1)
	  {
			Music_Off=!Music_Off;
			if(Music_Off)
				Music_Flag=MUSIC_FINISH;
			else
			{
				Cycle_Flag=!Cycle_Flag;
				Music_Flag=MUSIC_BEGIN;
			}
			OLED_RST_Flag=1;
			oled_t_ms=0;
		  g_iButtonState_Button3=0;
	  }

		
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
void motor_pwm(uint8_t motor,int16_t pwm){
	if(motor == 1)			//motor：LF 2  1 RF
	{										//				            pwm[-1000,1000] 
		if(pwm<0)//clockwise       LB 3  4 RB
		{
			if(pwm<-1000)
				pwm=-1000;
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,1000+pwm);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,1000);
		}
		else//anticlockwise
		{
			if(pwm>1000)
				pwm=1000;
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,1000);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,1000-pwm);
		}
	}
	else if(motor == 2)
	{
		if(pwm<0)//clockwise
		{
			if(pwm<-1000)
				pwm=-1000;
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,1000+pwm);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,1000);
		}
		else//anticlockwise
		{
			if(pwm>1000)
				pwm=1000;
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,1000);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,1000-pwm);
		}
	}
}

void motor_1khz(int16_t motor_L,int16_t motor_R)
{
	motor_pwm(1,motor_L);
	motor_pwm(2,motor_R);
}

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
		__HAL_TIM_SET_COMPARE(&NOTE_PWM_TIM,NOTE_PWM_CHANNEL,autoReload>>volumeLevel);
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
			if(!Cycle_Flag)
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
	/*判断当进入定时器11中断时*/
	if(htim->Instance == TIM11)
	{
		t_ms++;
		oled_t_ms++;
		t_Run_Flag++;
		t_Debug_Flag++;
		t_ms_stop++;
		OLED_Refresh++;
		if(t_ms>=500)
		{
			t_500ms++;
			t_ms=0;
			printf("hello world\n");
		}

		
/*******************************************************************************************************************/
		if( HAL_GPIO_ReadPin(UKEY_GPIO_Port,UKEY_Pin) == GPIO_PIN_RESET )//如果引脚检测到          低电平
		{
			iButtonCount_UKEY++; //按键按下，计数iButtonCount加1
			if(iButtonCount_UKEY>=7) //1ms中断服务函数里运行一次，iButtonCount大于等于16，即按键已稳定按下16ms
			{
				if(iButtonFlag_UKEY==0) //判断有没有重按键，1为有，0为没有
				{
					g_iButtonState_UKEY=1; //设置按键标志
					iButtonCount_UKEY=0;
					iButtonFlag_UKEY=1; //设置重按键标志
				}
				else //如果重按键，则重新计数
					iButtonCount_UKEY=0;
			}
			else //如果没有稳定按下7ms，则代表没有按下按键
				g_iButtonState_UKEY=0;
		}
		else //如果一直无检测到低电平，即一直无按键按下
		{
			iButtonCount_UKEY=0; //清零iButtonCount
			g_iButtonState_UKEY=0; //清除按键标志
			iButtonFlag_UKEY=0; //清除重按键标志
		}
		if( HAL_GPIO_ReadPin(Button1_GPIO_Port,Button1_Pin) == GPIO_PIN_SET )//如果引脚检测到          高电平
		{
			iButtonCount_Button1++; //按键按下，计数iButtonCount加1
			if(iButtonCount_Button1>=7) //1ms中断服务函数里运行一次，iButtonCount大于等于7，即按键已稳定按下7ms
			{
				if(iButtonFlag_Button1==0) //判断有没有重按键，1为有，0为没有
				{
					g_iButtonState_Button1=1; //设置按键标志
					iButtonCount_Button1=0;
					iButtonFlag_Button1=1; //设置重按键标志
				}
				else //如果重按键，则重新计数
					iButtonCount_Button1=0;
			}
			else //如果没有稳定按下7ms，则代表没有按下按键
				g_iButtonState_Button1=0;
		}
		else //如果一直无检测到低电平，即一直无按键按下
		{
			iButtonCount_Button1=0; //清零iButtonCount
			g_iButtonState_Button1=0; //清除按键标志
			iButtonFlag_Button1=0; //清除重按键标志
		}
		if( HAL_GPIO_ReadPin(Button2_GPIO_Port,Button2_Pin) == GPIO_PIN_SET )//如果引脚检测到          高电平
		{
			iButtonCount_Button2++; //按键按下，计数iButtonCount加1
			if(iButtonCount_Button2>=7) //1ms中断服务函数里运行一次，iButtonCount大于等于7，即按键已稳定按下7ms
			{
				if(iButtonFlag_Button2==0) //判断有没有重按键，1为有，0为没有
				{
					g_iButtonState_Button2=1; //设置按键标志
					iButtonCount_Button2=0;
					iButtonFlag_Button2=1; //设置重按键标志
				}
				else //如果重按键，则重新计数
					iButtonCount_Button2=0;
			}
			else //如果没有稳定按下7ms，则代表没有按下按键
				g_iButtonState_Button2=0;
		}
		else //如果一直无检测到低电平，即一直无按键按下
		{
			iButtonCount_Button2=0; //清零iButtonCount
			g_iButtonState_Button2=0; //清除按键标志
			iButtonFlag_Button2=0; //清除重按键标志
		}
		if( HAL_GPIO_ReadPin(Button3_GPIO_Port,Button3_Pin) == GPIO_PIN_SET )//如果引脚检测到          高电平
		{
			iButtonCount_Button3++; //按键按下，计数iButtonCount加1
			if(iButtonCount_Button3>=7) //1ms中断服务函数里运行一次，iButtonCount大于等于7，即按键已稳定按下7ms
			{
				if(iButtonFlag_Button3==0) //判断有没有重按键，1为有，0为没有
				{
					g_iButtonState_Button3=1; //设置按键标志
					iButtonCount_Button3=0;
					iButtonFlag_Button3=1; //设置重按键标志
				}
				else //如果重按键，则重新计数
					iButtonCount_Button3=0;
			}
			else //如果没有稳定按下7ms，则代表没有按下按键
				g_iButtonState_Button3=0;
		}
		else //如果一直无检测到低电平，即一直无按键按下
		{
			iButtonCount_Button3=0; //清零iButtonCount
			g_iButtonState_Button3=0; //清除按键标志
			iButtonFlag_Button3=0; //清除重按键标志
		}
	}
	 if(htim == &NOTE_CONTINUE_TIM)
  {
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
