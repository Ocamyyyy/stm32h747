/**
  ******************************************************************************
  * @file    DCMI/DCMI_SnapshotMode/CM7/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describe how to configure the camera interface (DCMI) in snapshot
  *          mode to handle a single image capture in QVGA (320x240) resolution and RGB565
  *          format and display the obtained image on LCD screen.
  *          This is the main program for Cortex-M7.    
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "usart.h"
#include "gpio.h"
#include "app_x-cube-ai.h"
/* Private includes*/
#include <stdio.h>

#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "network.h"
#include "network_data.h"


/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup DCMI_SnapshotMode
  * @{
  */

/* Private typedef -----------------------------------------------------------*//* Private typedef -----------------------------------------------------------*/
extern LTDC_HandleTypeDef hlcd_ltdc;
static DMA2D_HandleTypeDef           hdma2d;
extern DSI_HandleTypeDef hlcd_dsi;
DSI_VidCfgTypeDef hdsivideo_handle;
DSI_CmdCfgTypeDef CmdCfg;
DSI_LPCmdTypeDef LPCmd;
DSI_PLLInitTypeDef dsiPllInit;
static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
/* Private define ------------------------------------------------------------*/

#define VSYNC               1 
#define VBP                 1 
#define VFP                 1
#define VACT                480
#define HSYNC               1
#define HBP                 1
#define HFP                 1
#define HACT                800

#define LAYER0_ADDRESS               (LCD_FRAME_BUFFER)
/* Private define ------------------------------------------------------------*/
#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Camera resolution is QVGA (320x240) */
static uint32_t   CameraResX = QVGA_RES_X;
static uint32_t   CameraResY = QVGA_RES_Y;
static uint32_t   LcdResX    = 0;
static uint32_t   LcdResY    = 0;

__IO uint32_t camera_frame_ready = 0;
__IO uint32_t lcd_frame_ready = 0;

/* AI buffer IO handlers */
ai_float in_data[AI_NETWORK_IN_1_SIZE];  //对齐，起始地址是4的整数倍，实际就是按4字节对齐
ai_float out_data[AI_NETWORK_OUT_1_SIZE];
ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
ai_buffer *ai_input;
ai_buffer *ai_output;
ai_handle network = AI_HANDLE_NULL;

char color[10][6]={"blue","black","red","yellow","green","purple"};
uint8_t command[7];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void DMA2D_ConvertFrameToARGB8888(void *pSrc, void *pDst, uint32_t xsize, uint32_t ysize);
static void DMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);
void Error_Handler(void);
static uint8_t LCD_Init(void);
void LTDC_Init(void);
static void LCD_LayertInit(uint16_t LayerIndex, uint32_t Address);
static int32_t DSI_IO_Write(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size);
static int32_t DSI_IO_Read(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size);
int32_t LCD_GetXSize(uint32_t Instance, uint32_t *XSize);
int32_t LCD_GetYSize(uint32_t Instance, uint32_t *YSize);
void LCD_MspInit(void);
/* Exported functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
	const LCD_UTILS_Drv_t LCD_UTIL_Driver =
{
  BSP_LCD_DrawBitmap,
  BSP_LCD_FillRGBRect,
  BSP_LCD_DrawHLine,
  BSP_LCD_DrawVLine,
  BSP_LCD_FillRect,
  BSP_LCD_ReadPixel,
  BSP_LCD_WritePixel,
  BSP_LCD_GetXSize,
  BSP_LCD_GetYSize,
  BSP_LCD_SetActiveLayer,
  BSP_LCD_GetPixelFormat
};

int main(void)
{
  /* System Init, System clock, voltage scaling and L1-Cache configuration are done by CPU1 (Cortex-M7) 
     in the meantime Domain D2 is put in STOP mode(Cortex-M4 in deep-sleep)
  */
  char Tx_buf[50];
  int Tx_buf_len = 0;
  /* Configure the MPU attributes as Write Through for SDRAM*/
  MPU_Config();
  
  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  
  CameraResX = QVGA_RES_X;
  CameraResY = QVGA_RES_Y;
	

  /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 400 MHz */
  SystemClock_Config();
  PeriphCommonClock_Config();
	
  /* When system initialization is finished, Cortex-M7 could wakeup (when needed) the Cortex-M4  by means of 
     HSEM notification or by any D2 wakeup source (SEV,EXTI..)   */   

/* Initialize all configured peripherals */
  MX_GPIO_Init();
	MX_CRC_Init();
  MX_USART1_UART_Init();
	MX_UART8_Init();
  /* USER CODE BEGIN 2 */
  Tx_buf_len = sprintf(Tx_buf, "\r\nSTM32 X-Cube-AI test\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
	/* LEDs initialization */  
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);
	
	/* Initialize the SDRAM */
  BSP_SDRAM_Init(0);
  
  /*##- Initialise the LCD #################################################*/
  /* Proceed to LTDC, DSI and LCD screen initialization with the configuration filled in above */
  /* for stageNb == 1 */
  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  /* Get LCD width and height*/  
  BSP_LCD_GetXSize(0,&LcdResX);
  BSP_LCD_GetYSize(0,&LcdResY);  

  /* Reset and power down camera to be sure camera is Off prior start testing BSP */
  BSP_CAMERA_PwrDown(0);
  
  if( LCD_Init() != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
    /* Set the LCD Context */
  Lcd_Ctx[0].ActiveLayer = 0;
  Lcd_Ctx[0].PixelFormat = LCD_PIXEL_FORMAT_ARGB8888;
  Lcd_Ctx[0].BppFactor = 4; /* 4 Bytes Per Pixel for ARGB8888 */  
  Lcd_Ctx[0].XSize = 800;  
  Lcd_Ctx[0].YSize = 480;
  
  /* Disable DSI Wrapper in order to access and configure the LTDC */
  __HAL_DSI_WRAPPER_DISABLE(&hlcd_dsi);

  /* Initialize LTDC layer 0 iused for Hint */  
  LCD_LayertInit(0, LCD_FRAME_BUFFER); 
  UTIL_LCD_SetFuncDriver(&LCD_UTIL_Driver);

  /* Enable DSI Wrapper so DSI IP will drive the LTDC */
  __HAL_DSI_WRAPPER_ENABLE(&hlcd_dsi);  

  /* Prepare using DMA2D the 800x480 LCD frame buffer to display background color black */
  /* and title of the example                                                           */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font16);

  /* Print example description */
  UTIL_LCD_DisplayStringAt(0, 440, (uint8_t *)"DCMI Snapshot", CENTER_MODE);
  HAL_DSI_Refresh(&hlcd_dsi);
	HAL_Delay(2000);	
	
  /*## Camera Initialization and capture start ############################*/
  /* Initialize the Camera in QVGA mode */
  if(BSP_CAMERA_Init(0,CAMERA_R320x240,CAMERA_PF_RGB565) != BSP_ERROR_NONE)
  {
    Error_Handler(); 
  }

  /* Wait 1s to let auto-loops in the camera module converge and lead to correct exposure */
  HAL_Delay(1000);
  
  /* Start the Camera Snapshot Capture */
  BSP_CAMERA_Start(0,(uint8_t *)CAMERA_FRAME_BUFFER,CAMERA_MODE_SNAPSHOT);

  /* Wait until camera frame is ready : DCMI Frame event */
  while(camera_frame_ready == 0)
  {
  }
  
  /* Stop the camera to avoid having the DMA2D work in parallel of Display */
  /* which cause perturbation of LTDC                                      */
  BSP_CAMERA_Stop(0);

  /* Copy the Camera captured frame to the LCD display buffer using DMA2D */
  DMA2D_ConvertFrameToARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER), (uint32_t *)(LCD_FRAME_BUFFER), CameraResX, CameraResY);
	/* Wait until DMA2D transfer complete : LCD frame is ready  */
  while(lcd_frame_ready == 0)
  {
  }
  //HAL_DSI_Refresh(&hlcd_dsi);
	HAL_Delay(1000);
	
	UTIL_LCD_ClearStringLine(460);
  UTIL_LCD_DisplayStringAt(0, 460, (uint8_t *)"Capture OK", CENTER_MODE);
	HAL_DSI_Refresh(&hlcd_dsi);
	HAL_Delay(1000);	
	
	/* AI network initialization */
	MX_X_CUBE_AI_Init();
  //AI buffer IO handlers
	
  ai_error err;
  ai_network_report report;
	//Initialize network 
  const ai_handle acts[] = { activations };

  err = ai_network_create_and_init(&network, acts, NULL);
  if (err.type != AI_ERROR_NONE) {
		Tx_buf_len = sprintf(Tx_buf, "Error: could not create NN instance\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
    return -1;
  }
  else{
	  Tx_buf_len = sprintf(Tx_buf, "success in create NN instance\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
  }

  if (ai_network_get_report(network, &report) != true) {
    Tx_buf_len = sprintf(Tx_buf, "ai get report error\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
    return -1;
	}
	Tx_buf_len = sprintf(Tx_buf, "Model name      : %s\n", report.model_name);
  HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
	ai_input = &report.inputs[0];
  ai_output = &report.outputs[0];
	
	// Input and output 
	ai_input = ai_network_inputs_get(network, NULL);    //这两句可以不要，因为前面已经调用过ai_network_get_report()并赋值了。
  ai_output = ai_network_outputs_get(network, NULL);

  //link inputs and outputs to the model
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  ai_i32 nbatch;
  
  /* resize img as network input*/
	int j=0;
	uint8_t*img=(uint8_t *)CAMERA_FRAME_BUFFER;
	for (int i = 0; i < CameraResX * CameraResY * 3; i++)
        if (i % 12 == 2 && (i / (320 * 3)) % 3 == 0) {
            in_data[j++] = (float)(img[i])/255;
        }
	for (int i = 0; i < CameraResX * CameraResY * 3; i++)
        if (i % 12 == 1 && (i / (320 * 3)) % 3 == 0) {
            in_data[j++] = (float)(img[i])/255;
        }
	for (int i = 0; i < CameraResX * CameraResY * 3; i++)
        if (i % 12 == 0 && (i / (320 * 3)) % 3 == 0) {
            in_data[j++] = (float)(img[i])/255;
        }
	
  
		//Tx_buf_len = sprintf(Tx_buf, "hello world%u\r\n\n",*(img+(320*237+316)*3));
		//Tx_buf_len = sprintf(Tx_buf, "hello world\r\n\n");
		//HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
		//Tx_buf_len = sprintf(Tx_buf, "hello world%f\r\n\n",in_data[19199]);
		//HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);

		/*float x = 0.0f;
		
		x = x + 0.1f;
		for (uint32_t i = 0; i < AI_NETWORK_IN_1_SIZE; i++)
	  {
		  in_data[i] = (ai_float)x;
		}*/
		
    //run the network		
		nbatch = ai_network_run(network, &ai_input[0], &ai_output[0]);
		if (nbatch != 1) {
			Tx_buf_len = sprintf(Tx_buf, "Error: could not run inference\r\n");
			HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
	  }
		else{
			Tx_buf_len = sprintf(Tx_buf, "Run successfully\r\n");
			HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
		}

		//transfer output
		Tx_buf_len = sprintf(Tx_buf, "Inference Confidence output...\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
		//Tx_buf_len = sprintf(Tx_buf, "%f,", out_data[0]);
		//HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
		float max=out_data[0];
		int max_id=0;
	  for (int i = 1; i < AI_NETWORK_OUT_1_SIZE; i++){ 
		  //Tx_buf_len = sprintf(Tx_buf, "%f,", out_data[i]);
		  //HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);
			if(out_data[i]>max){
				max=out_data[i];
				max_id=i;
			}
	  }
		Tx_buf_len = sprintf(Tx_buf, "\r\nup is %s\r\n",color[max_id]);
    HAL_UART_Transmit(&huart1, (uint8_t *)Tx_buf, Tx_buf_len, 100);	
		command[0]=0x55;
		command[1]=0x55;
		command[2]=5;
		command[3]=6;
		command[5]=1;
		command[6]=0;
		UTIL_LCD_ClearStringLine(420);
		if (max_id!=0){
			if (max_id==5)
				command[4]=1;
			else
				command[4]=0;
		  HAL_UART_Transmit(&huart8, (uint8_t *)command, 7, 100);
			HAL_Delay (200);
			UTIL_LCD_DisplayStringAt(0, 420, (uint8_t *)"up is false", CENTER_MODE);
		}
		else
			UTIL_LCD_DisplayStringAt(0, 420, (uint8_t *)"up is right", CENTER_MODE);
		HAL_DSI_Refresh(&hlcd_dsi);
	  HAL_Delay(1000);	
		while (1)
  {
    BSP_LED_Toggle(LED_GREEN);
		
		HAL_Delay (200);
		
  }
}

/**
  * @brief  Gets the LCD X size.
  * @param  Instance  LCD Instance
  * @param  XSize     LCD width
  * @retval BSP status
  */
int32_t LCD_GetXSize(uint32_t Instance, uint32_t *XSize)
{
  *XSize = Lcd_Ctx[0].XSize;
 
  return BSP_ERROR_NONE;
}

/**
  * @brief  Gets the LCD Y size.
  * @param  Instance  LCD Instance
  * @param  YSize     LCD Height
  * @retval BSP status
  */
int32_t LCD_GetYSize(uint32_t Instance, uint32_t *YSize)
{
  *YSize = Lcd_Ctx[0].YSize;
 
  return BSP_ERROR_NONE;
}

/**
  * @brief  Initializes the DSI LCD. 
  * The ititialization is done as below:
  *     - DSI PLL ititialization
  *     - DSI ititialization
  *     - LTDC ititialization
  *     - OTM8009A LCD Display IC Driver ititialization
  * @param  None
  * @retval LCD state
  */
static uint8_t LCD_Init(void)
{
  DSI_PHY_TimerTypeDef  PhyTimings;
  
  OTM8009A_IO_t              IOCtx;
  static OTM8009A_Object_t   OTM8009AObj;
  static void                *Lcd_CompObj = NULL;
  
  /* Toggle Hardware Reset of the DSI LCD using
     its XRES signal (active low) */
  BSP_LCD_Reset(0);
  
  /* Call first MSP Initialize only in case of first initialization
  * This will set IP blocks LTDC, DSI and DMA2D
  * - out of reset
  * - clocked
  * - NVIC IRQ related to IP blocks enabled
  */
  LCD_MspInit();
 
  /* LCD clock configuration */
  /* LCD clock configuration */
  /* PLL3_VCO Input = HSE_VALUE/PLL3M = 5 Mhz */
  /* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 800 Mhz */
  /* PLLLCDCLK = PLL3_VCO Output/PLL3R = 800/19 = 42 Mhz */
  /* LTDC clock frequency = PLLLCDCLK = 42 Mhz */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M = 5;    
  PeriphClkInitStruct.PLL3.PLL3N = 160;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;  
  PeriphClkInitStruct.PLL3.PLL3R = 19;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);   
  
  /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
  hlcd_dsi.Instance = DSI;
  
  HAL_DSI_DeInit(&(hlcd_dsi));
  
  dsiPllInit.PLLNDIV  = 100;
  dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV5;
  dsiPllInit.PLLODF  = DSI_PLL_OUT_DIV1;  

  hlcd_dsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
  hlcd_dsi.Init.TXEscapeCkdiv = 0x4;
  
  
  HAL_DSI_Init(&(hlcd_dsi), &(dsiPllInit));
    
  /* Configure the DSI for Command mode */
  CmdCfg.VirtualChannelID      = 0;
  CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
  CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
  CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
  CmdCfg.ColorCoding           = DSI_RGB888;
  CmdCfg.CommandSize           = HACT;
  CmdCfg.TearingEffectSource   = DSI_TE_DSILINK;
  CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
  CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
  CmdCfg.AutomaticRefresh      = DSI_AR_DISABLE;
  CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
  HAL_DSI_ConfigAdaptedCommandMode(&hlcd_dsi, &CmdCfg);
  
  LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_ENABLE;
  LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_ENABLE;
  LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_ENABLE;
  LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_ENABLE;
  LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_ENABLE;
  LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_ENABLE;
  LPCmd.LPGenLongWrite        = DSI_LP_GLW_ENABLE;
  LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_ENABLE;
  LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_ENABLE;
  LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_ENABLE;
  LPCmd.LPDcsLongWrite        = DSI_LP_DLW_ENABLE;
  HAL_DSI_ConfigCommand(&hlcd_dsi, &LPCmd);

  /* Initialize LTDC */
  LTDC_Init();
  
  /* Start DSI */
  HAL_DSI_Start(&(hlcd_dsi));
 
  /* Configure DSI PHY HS2LP and LP2HS timings */
  PhyTimings.ClockLaneHS2LPTime = 35;
  PhyTimings.ClockLaneLP2HSTime = 35;
  PhyTimings.DataLaneHS2LPTime = 35;
  PhyTimings.DataLaneLP2HSTime = 35;
  PhyTimings.DataLaneMaxReadTime = 0;
  PhyTimings.StopWaitTime = 10;
  HAL_DSI_ConfigPhyTimer(&hlcd_dsi, &PhyTimings);   
    
  /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
  */
  IOCtx.Address     = 0;
  IOCtx.GetTick     = BSP_GetTick;
  IOCtx.WriteReg    = DSI_IO_Write;
  IOCtx.ReadReg     = DSI_IO_Read;
  OTM8009A_RegisterBusIO(&OTM8009AObj, &IOCtx);
  Lcd_CompObj=(&OTM8009AObj);
  OTM8009A_Init(Lcd_CompObj, OTM8009A_COLMOD_RGB888, LCD_ORIENTATION_LANDSCAPE);
  
  LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_DISABLE;
  LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_DISABLE;
  LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_DISABLE;
  LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_DISABLE;
  LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_DISABLE;
  LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_DISABLE;
  LPCmd.LPGenLongWrite        = DSI_LP_GLW_DISABLE;
  LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_DISABLE;
  LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_DISABLE;
  LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_DISABLE;
  LPCmd.LPDcsLongWrite        = DSI_LP_DLW_DISABLE;
  HAL_DSI_ConfigCommand(&hlcd_dsi, &LPCmd);
  
  HAL_DSI_ConfigFlowControl(&hlcd_dsi, DSI_FLOW_CONTROL_BTA);
  HAL_DSI_ForceRXLowPower(&hlcd_dsi, ENABLE);  
  
  return BSP_ERROR_NONE;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void LTDC_Init(void)
{
  /* DeInit */
  hlcd_ltdc.Instance = LTDC;
  HAL_LTDC_DeInit(&hlcd_ltdc);
  
  /* LTDC Config */
  /* Timing and polarity */
  hlcd_ltdc.Init.HorizontalSync = HSYNC;
  hlcd_ltdc.Init.VerticalSync = VSYNC;
  hlcd_ltdc.Init.AccumulatedHBP = HSYNC+HBP;
  hlcd_ltdc.Init.AccumulatedVBP = VSYNC+VBP;
  hlcd_ltdc.Init.AccumulatedActiveH = VSYNC+VBP+VACT;
  hlcd_ltdc.Init.AccumulatedActiveW = HSYNC+HBP+HACT;
  hlcd_ltdc.Init.TotalHeigh = VSYNC+VBP+VACT+VFP;
  hlcd_ltdc.Init.TotalWidth = HSYNC+HBP+HACT+HFP;
  
  /* background value */
  hlcd_ltdc.Init.Backcolor.Blue = 0;
  hlcd_ltdc.Init.Backcolor.Green = 0;
  hlcd_ltdc.Init.Backcolor.Red = 0;
  
  /* Polarity */
  hlcd_ltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hlcd_ltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hlcd_ltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hlcd_ltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hlcd_ltdc.Instance = LTDC;

  HAL_LTDC_Init(&hlcd_ltdc);
}

/**
  * @brief  Initializes the LCD layers.
  * @param  LayerIndex: Layer foreground or background
  * @param  FB_Address: Layer frame buffer
  * @retval None
  */
static void LCD_LayertInit(uint16_t LayerIndex, uint32_t Address)
{
  LTDC_LayerCfgTypeDef  layercfg;

  /* Layer Init */
  layercfg.WindowX0 = 0;
  layercfg.WindowX1 = Lcd_Ctx[0].XSize ;
  layercfg.WindowY0 = 0;
  layercfg.WindowY1 = Lcd_Ctx[0].YSize; 
  layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  layercfg.FBStartAdress = Address;
  layercfg.Alpha = 255;
  layercfg.Alpha0 = 0;
  layercfg.Backcolor.Blue = 0;
  layercfg.Backcolor.Green = 0;
  layercfg.Backcolor.Red = 0;
  layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  layercfg.ImageWidth = Lcd_Ctx[0].XSize;
  layercfg.ImageHeight = Lcd_Ctx[0].YSize;
  
  HAL_LTDC_ConfigLayer(&hlcd_ltdc, &layercfg, LayerIndex); 
}

/**
  * @brief  DCS or Generic short/long write command
  * @param  ChannelNbr Virtual channel ID
  * @param  Reg Register to be written
  * @param  pData pointer to a buffer of data to be write
  * @param  Size To precise command to be used (short or long)
  * @retval BSP status
  */
static int32_t DSI_IO_Write(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Size <= 1U)
  {
    if(HAL_DSI_ShortWrite(&hlcd_dsi, ChannelNbr, DSI_DCS_SHORT_PKT_WRITE_P1, Reg, (uint32_t)pData[Size]) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }
  else
  {
    if(HAL_DSI_LongWrite(&hlcd_dsi, ChannelNbr, DSI_DCS_LONG_PKT_WRITE, Size, (uint32_t)Reg, pData) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DCS or Generic read command
  * @param  ChannelNbr Virtual channel ID
  * @param  Reg Register to be read
  * @param  pData pointer to a buffer to store the payload of a read back operation.
  * @param  Size  Data size to be read (in byte).
  * @retval BSP status
  */
static int32_t DSI_IO_Read(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_DSI_Read(&hlcd_dsi, ChannelNbr, pData, Size, DSI_DCS_SHORT_PKT_READ, Reg, pData) != HAL_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

void LCD_MspInit(void)
{
  /** @brief Enable the LTDC clock */
  __HAL_RCC_LTDC_CLK_ENABLE();

  /** @brief Toggle Sw reset of LTDC IP */
  __HAL_RCC_LTDC_FORCE_RESET();
  __HAL_RCC_LTDC_RELEASE_RESET();

  /** @brief Enable the DMA2D clock */
  __HAL_RCC_DMA2D_CLK_ENABLE();

  /** @brief Toggle Sw reset of DMA2D IP */
  __HAL_RCC_DMA2D_FORCE_RESET();
  __HAL_RCC_DMA2D_RELEASE_RESET();

  /** @brief Enable DSI Host and wrapper clocks */
  __HAL_RCC_DSI_CLK_ENABLE();

  /** @brief Soft Reset the DSI Host and wrapper */
  __HAL_RCC_DSI_FORCE_RESET();
  __HAL_RCC_DSI_RELEASE_RESET();

  /** @brief NVIC configuration for LTDC interrupt that is now enabled */
  HAL_NVIC_SetPriority(LTDC_IRQn, 9, 0xf);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);

  /** @brief NVIC configuration for DMA2D interrupt that is now enabled */
  HAL_NVIC_SetPriority(DMA2D_IRQn, 9, 0xf);
  HAL_NVIC_EnableIRQ(DMA2D_IRQn);

  /** @brief NVIC configuration for DSI interrupt that is now enabled */
  HAL_NVIC_SetPriority(DSI_IRQn, 9, 0xf);
  HAL_NVIC_EnableIRQ(DSI_IRQn);
}

/**
  * @brief  Display Example description.
  * @param  None
  * @retval None
  */
static void LCD_BriefDisplay(void)
{
  UTIL_LCD_SetFont(&Font24);  
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE); 
  UTIL_LCD_FillRect(0, 0, 800, 112, UTIL_LCD_COLOR_BLUE); 
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(0, 112, 800, 368, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);  
  UTIL_LCD_DisplayStringAtLine(1, (uint8_t *)"           LCD_DSI_CmdMode_DoubleBuffering");
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAtLine(4, (uint8_t *)"This example shows how to display images on LCD DSI using two buffers");
  UTIL_LCD_DisplayStringAtLine(5, (uint8_t *)"one for display and the other for draw");  
}

/**
  * @brief  Converts a line to an ARGB8888 pixel format.
  * @param  pSrc: Pointer to source buffer
  * @param  pDst: Output color
  * @param  ColorMode: Input color mode
  * @retval None
  */
static void DMA2D_ConvertFrameToARGB8888(void *pSrc, void *pDst, uint32_t xsize, uint32_t ysize)
{
  uint32_t xPos, yPos, destination;   
  
  /* Calculate the destination transfer address */
  xPos = (LcdResX  - xsize)/2;
  yPos = (LcdResY  - ysize)/2;  
  
  /* Enable DMA2D clock */
  __HAL_RCC_DMA2D_CLK_ENABLE();

  /* Configure the DMA2D Mode, Color Mode and output offset */
  hlcd_dma2d.Init.Mode         = DMA2D_M2M_PFC;
  hlcd_dma2d.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888; /* Output color out of PFC */
  hlcd_dma2d.Init.OutputOffset = LcdResX - xsize;
  hlcd_dma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No Output Alpha Inversion*/  
  hlcd_dma2d.Init.RedBlueSwap   = DMA2D_RB_REGULAR;     /* No Output Red & Blue swap */ 

  /* DMA2D Callbacks Configuration */
  hlcd_dma2d.XferCpltCallback  = DMA2D_TransferCompleteCallback;  

  /* Foreground Configuration */
  hlcd_dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hlcd_dma2d.LayerCfg[1].InputAlpha = 0xFF; /* fully opaque */
  hlcd_dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hlcd_dma2d.LayerCfg[1].InputOffset = 0;
  hlcd_dma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hlcd_dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */ 
  
  hlcd_dma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hlcd_dma2d) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1) != HAL_OK) 
    {
      Error_Handler();      
    }
  }
  else
  {
    Error_Handler();
  } 
  
  /* Calculate the destination transfer address */
  xPos = (LcdResX  - xsize)/2;
  yPos = (LcdResY  - ysize)/2;  
  
  destination = (uint32_t)pDst + (yPos * LcdResX + xPos) * ARGB8888_BYTE_PER_PIXEL;
  
  /* Starts the DMA2D transfer */
  if(HAL_DMA2D_Start_IT(&hlcd_dma2d, (uint32_t)pSrc, destination, xsize, ysize) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Camera line event callback
  * @param  None
  * @retval None
  */
void BSP_CAMERA_FrameEventCallback(uint32_t Instance)
{
  camera_frame_ready = 1;
}

/**
 * @brief  DMA2D Transfer completed callback
 * @param  hdma2d: DMA2D handle.
 * @retval None
 */
static void DMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d)
{
  lcd_frame_ready = 1;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
	
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
 /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)       
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
 */
 
  /*activate CSI clock mondatory for I/O Compensation Cell*/  
  __HAL_RCC_CSI_ENABLE() ;
    
  /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;
  
  /* Enables the I/O Compensation Cell */    
  HAL_EnableCompensationCell();  
}
/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 12;
  PeriphClkInitStruct.PLL2.PLL2P = 1;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Turn LED REDon */
  BSP_LED_On(LED_RED);
  while(1)
  {
  }
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  Configure the MPU attributes as Write Through for External SDRAM.
  * @note   The Base Address is 0xD0000000 .
  *         The Configured Region Size is 32MB because same as SDRAM size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

