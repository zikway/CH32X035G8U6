
#include "hw_pwm.h"
#include "sdk_api_pwm.h"
#include "ch32x035_tim.h"
#include "ch32x035_rcc.h"

bool sdk_api_pwm_set_low_high_time(const pin_map_t* p_map, uint32_t freq, uint8_t duty)
{
    //low_time, high time 的unit= 1/32M (second)
    //计算low_time
    uint32_t low_time, high_time;
    return true;
}

bool sdk_api_pwm_init(const pin_map_t* p_map,uint32_t freq, uint8_t duty)
{
		GPIO_InitTypeDef GPIO_InitStructure={0};
		TIM_OCInitTypeDef TIM_OCInitStructure={0};
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};
		uint32_t i;
		uint32_t j;
		uint32_t frequency;
		frequency = 48000000/freq/255;
		i=p_map->attribute;
		j=p_map->peripheral;
		if((p_map->pin == PB_11 && j == pwm_timer2) || (p_map->pin == PB_12) &&  j == pwm_timer2){
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
			GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE);
		}
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE );
		RCC_APB2PeriphClockCmd(get_gpio_rcc(p_map->pin), ENABLE);
		GPIO_InitStructure.GPIO_Pin = get_gpio_pin(p_map->pin);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(get_gpio_port(p_map->pin), &GPIO_InitStructure);

		TIM_TimeBaseInitStructure.TIM_Period = 255-1;
		TIM_TimeBaseInitStructure.TIM_Prescaler = frequency;
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_Pulse = duty;
		//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set; 
		if(i==pwm_ch1){
			TIM_OC1Init( TIM2, &TIM_OCInitStructure );
		}else if(i==pwm_ch2){
			TIM_OC2Init( TIM2, &TIM_OCInitStructure );
		}else if(i==pwm_ch3){
			TIM_OC3Init( TIM2, &TIM_OCInitStructure );
		}else if(i==pwm_ch4){
			TIM_OC4Init( TIM2, &TIM_OCInitStructure );
		}

		TIM_CtrlPWMOutputs(TIM2, ENABLE );
		if(i==pwm_ch1){
			TIM_OC1PreloadConfig( TIM2, TIM_OCPreload_Disable );
		}else if(i==pwm_ch2){
			TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Disable );
		}else if(i==pwm_ch3){
			TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Disable );
		}else if(i==pwm_ch4){
			TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Disable );
		}
		TIM_ARRPreloadConfig( TIM2, ENABLE );
		TIM_Cmd( TIM2, ENABLE );
	printf("pwm=%d init\r\n",i);
    return 1;
}

uint16_t sdk_api_pwm_set_pwm_duty(const pin_map_t* p_map, uint8_t duty)
{
	uint32_t i;
    uint32_t j;
    i=p_map->attribute;
    j=p_map->peripheral;
     switch(j){
	 	case pwm_timer1:
	 	if(pwm_ch1 == i){
	 		TIM1->CH1CVR=duty;
	 	}else if(pwm_ch2 == i){
	 		TIM1->CH2CVR=duty;
	 	}else if(pwm_ch3 == i){
	 		TIM1->CH3CVR=duty;
	 	}else if(pwm_ch4 == i){
	 		TIM1->CH4CVR=duty;
	 	}
	 	break;
	 	case pwm_timer2:
	 	if(pwm_ch1 == i){
	 		TIM2->CH1CVR=duty;
	 	}else if(pwm_ch2 == i){
	 		TIM2->CH2CVR=duty;
	 	}else if(pwm_ch3 == i){
	 		TIM2->CH3CVR=duty;
	 	}else if(pwm_ch4 == i){
	 		TIM2->CH4CVR=duty;
	 	}
	 	break;
	 	case pwm_timer3:
	 	if(pwm_ch1 == i){
	 		TIM3->CH1CVR=duty;
	 	}else if(pwm_ch2 == i){
	 		TIM3->CH2CVR=duty;
	 	}else if(pwm_ch3 == i){
	 		TIM3->CH3CVR=duty;
	 	}else if(pwm_ch4 == i){
	 		TIM3->CH4CVR=duty;
	 	}
	 	break;
	 }
    return true;
}

bool sdk_api_pwm_set_pwm_freq(const pin_map_t* p_map, uint32_t hz)
{
    uint32_t i;
	uint32_t j;
	uint32_t frequency;
    i=p_map->attribute;
    j=p_map->peripheral;
	frequency=48000000/250/hz;
	 switch(j){
	 	case pwm_timer1:
	 		TIM1->PSC=frequency;
	 	break;
	 	case pwm_timer2:
	 		TIM2->PSC=frequency;
	 	break;
	 	case pwm_timer3:
	 		TIM3->PSC=frequency;
	 	break;
	 }
    return true;
    return true;
}


bool sdk_api_pwm_deinit(const pin_map_t* p_map)
{
    return true;
}

