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
  /* USER CODE BEGIN 2 */
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
		
		  g_iButtonState_UKEY=0;
	  }
//	  else
//	  {
//		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
//	  }
	  if(g_iButtonState_Button1 == 1)
	  {
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
