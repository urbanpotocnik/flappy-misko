/*
 * periodic_services.h
 *
 *  Created on: 
 *      Author:
 */

#ifndef PERIODIC_SERVICES_H_
#define PERIODIC_SERVICES_H_

//    stm32g4xx_ll_tim.h
//
//    void LL_TIM_EnableCounter(TIM_TypeDef * TIMx)
//
//    void LL_TIM_EnableUpdateEvent(TIM_TypeDef * TIMx)
//    ALI
//    void LL_TIM_EnableIT_UPDATE(TIM_TypeDef * TIMx)
//
//    void LL_TIM_IsEnabledUpdateEvent(TIM_TypeDef * TIMx)
//    ALI
//    void LL_TIM_IsEnabledIT_UPDATE(TIM_TypeDef * TIMx)
//
//    void LL_TIM_IsActiveFlag_UPDATE(TIM_TypeDef * TIMx)
//
//    void LL_TIM_ClearFlag_UPDATE(TIM_TypeDef * TIMx)

// -------------------- Public definitions --------------------



// ---------------- Public function prototypes ----------------

void PSERV_init(void);
void PSERV_enable(void);
void PSERV_disable(void);
void PSERV_run_services_Callback(void);


#endif /* PERIODIC_SERVICES_H_ */
