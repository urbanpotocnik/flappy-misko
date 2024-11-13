/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// -------- Prekinitvam je potrebno dodati sistemsko funkcionalnost SCI vmesnika:

// Znotraj prekinitvene rutine USART3_IRQHandler() moramo biti sposobni poklicati,
// t.i. "callback" funkcije za sprejemanje novega podatka oziroma poÅ¡iljanje novega
// podatka preko serijskega vmesnika SCI. Te "callback" funkcije so pravzaprav del sistema,
// ki se odziva na prekinitve na strojnem nivoju. Vidite lahko, da tokrat pa
// niÅ¾je-nivojski strojni nivo (tj. USART enota) potrebuje storitve
// viÅ¡jega sistemskega nivoja (tj. medpomnilnikov SCI vmesnika):
//		- ob sprejemu Å¾elimo sprejeti podatek shraniti v sistemski SCI RX medpomnilnik
//		- pri poÅ¡iljanju podatkov Å¾elimo podatke jemati iz sistemskega SCI TX medpomnilnika


	#include "SCI.h"
	#include "periodic_services.h"
	#include "timing_utils.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc4;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc4);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt / USART3 wake-up interrupt through EXTI line 28.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */


	// Kot Å¾e reÄ?eno, je potrebno znotraj prekinitvene rutine ugotoviti, kateri od specifiÄ?nih
	// prekinitvenih dogodkov je sproÅ¾il globalno USART3 prekinitev in se potem ustrezno odzvati
	// na ta prekinitveni dogodek s "callback funkcijo".

	// Vir prekinitve ugotavljamo s pomoÄ?jo prekinitveno-statusnega registra USART vmesnika
	// ob pomoÄ?i LL funkcij.

	// Nujno pa je tudi, da preden se odzovemo na specifiÄ?ni vir prekinitve, preverimo,
	// Ä?e je ta specifiÄ?na prekinitev sploh omogoÄ?ena. ÄŒe tega ne bi storili, tvegamo,
	// da se na prekinitveni dogodek odzovemo z napaÄ?no prekinitveno "callback" rutino.





	// ------ Odzivanje na novo-sprejeti podatek (zastavica RXNE = Receive data register not empty) -------


		// Najprej preverimo, Ä?e je ta specifiÄ?na prekinitev sploh omogoÄ?ena. Uporabimo LL funkcijo.
		if( LL_USART_IsEnabledIT_RXNE_RXFNE (USART3) )		// sploh omogoÄ?ena prekinitev ob RXNE?
		{
			// ÄŒe je prekinitev omogoÄ?ena, potem preverimo Å¡e, Ä?e je postavljena ustrezna zastavica.
			if( LL_USART_IsActiveFlag_RXNE_RXFNE (USART3) )	// postavljena zastavica RXNE?
			{
				// ÄŒe je ta specifiÄ?na prekinitev omogoÄ?ena in Ä?e je postavljena zastavica tega specifiÄ?nega
				// prekinitvenega dogodka, potem se odzovemo s klicem ustrezne "callback" rutine.

				SCI_receive_char_Callback();

				// V vednost: zastavica RXNE se zbriÅ¡e avtomatsko, ko preberemo sprejemni register RDR.
			}
		}




	// ------ Odzivanje na sprostitev oddajnega podatkovnega registra TDR (zastavica TXE = Transmitter Empty) -------

		// Najprej preverimo, Ä?e je ta specifiÄ?na prekinitev sploh omogoÄ?ena. Uporabimo LL funkcijo.
		if( LL_USART_IsEnabledIT_TXE_TXFNF (USART3) )		// sploh omogoÄ?ena prekinitev ob TXE?
		{
			// ÄŒe je prekinitev omogoÄ?ena, potem preverimo Å¡e, Ä?e je postavljena ustrezna zastavica.
			if( LL_USART_IsActiveFlag_TXE_TXFNF(USART3) )		// postavljena zastavica TXE?
			{
				// ÄŒe je ta specifiÄ?na prekinitev omogoÄ?ena in Ä?e je postavljena zastavica tega specifiÄ?nega
				// prekinitvenega dogodka, potem se odzovemo s klicem ustrezne "callback" rutine.

				SCI_transmit_char_Callback();

				// V vednost: zastavica TXE se zbriÅ¡e avtomatsko, ko piÅ¡emo v oddajni podatkovni register TDR.
			}
		}




  /* USER CODE END USART3_IRQn 0 */
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC3 channel underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

	if(LL_TIM_IsEnabledIT_UPDATE(TIM6))
	{
		if(LL_TIM_IsActiveFlag_UPDATE(TIM6))
		{
			PSERV_run_services_Callback();
			LL_TIM_ClearFlag_UPDATE(TIM6);
		}
	}

  /* USER CODE END TIM6_DAC_IRQn 0 */

  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

