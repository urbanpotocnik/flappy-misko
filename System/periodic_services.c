/*
 * periodic_services.c
 *
 *  Created on: 
 *      Author: 
 */

#include "LED.h"
#include "kbd.h"
#include "SCI.h"
#include "periodic_services.h"
#include "timing_utils.h"
#include "buf.h"
#include "stm32g4xx_ll_tim.h"

typedef struct
{

	TIM_TypeDef * TIM;

}periodic_services_handle_t;

periodic_services_handle_t periodic_services;



void PSERV_init(void)
{
	periodic_services.TIM = TIM6;
	LL_TIM_EnableCounter(periodic_services.TIM);
}

void PSERV_enable(void)
{
	LL_TIM_EnableIT_UPDATE(periodic_services.TIM);
}

void PSERV_disable(void)
{
	LL_TIM_DisableIT_UPDATE(periodic_services.TIM);
}

void PSERV_run_services_Callback(void)
{
	KBD_scan();
	
}
