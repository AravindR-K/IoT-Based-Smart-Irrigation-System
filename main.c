#include "stm32f4xx.h"

int tempReading = 0;
int moistReading = 0;
const int tempThreshold = 1000;    
const int moistThreshold = 3000;

void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 1600; i++)
        __NOP();
}

uint16_t ADC_Read(uint8_t channel);

int main(void)
{
    // Enable clocks 
    RCC->AHB1ENR |= (1U << 0);  // GPIOA clock (ADC + USART + switch)
    RCC->AHB1ENR |= (1U << 2);  // GPIOC clock (LED)
    RCC->APB2ENR |= (1U << 8);  // ADC1 clock
    RCC->APB2ENR |= (1U << 4);  // USART1 clock
    RCC->APB2ENR |= (1U << 14); //  SYSCFG clock (for EXTI)

    // Configure PC14 as output (LED) 
    GPIOC->MODER &= ~(3U << 28);
    GPIOC->MODER |=  (1U << 28);
	
    // Configure PC13 as output (LED) 
    GPIOC->MODER &= ~(3U << 26);
    GPIOC->MODER |=  (1U << 26);
		

    // Configure PA9 as AF7 (USART1_TX) 
    GPIOA->MODER &= ~(3U << 18);
    GPIOA->MODER |=  (2U << 18);
    GPIOA->AFR[1] &= ~(0xF << 4);
    GPIOA->AFR[1] |=  (7U << 4);

    //  Configure PA0 as input (button / switch) 
    GPIOA->MODER &= ~(3U << 0);    // input mode
    GPIOA->PUPDR &= ~(3U << 0);
    GPIOA->PUPDR |=  (1U << 0);    // pull-up

    // --- Configure ADC pins PA2, PA3 as analog ---
    GPIOA->MODER |= (3U << (2 * 2)); 
    GPIOA->MODER |= (3U << (3 * 2));
    GPIOA->PUPDR &= ~(3U << (2 * 2));
    GPIOA->PUPDR &= ~(3U << (3 * 2));

    // USART1 configuration 
    USART1->BRR = 0x008B;      // 115200 bps @ 16 MHz
    USART1->CR1 |= (1 << 3);   // TE enable
    USART1->CR1 |= (1 << 13);  // USART enable

    // ADC1 configuration 
    ADC->CCR = 0;
    ADC1->CR1 = 0;
    ADC1->CR2 = 0;
    ADC1->SQR1 = 0;
    ADC1->SMPR2 |= (3U << 6);
    ADC1->SMPR2 |= (3U << 9);
    ADC1->CR2 |= 1;

    // EXTI0 (NVIC) configuration 
    SYSCFG->EXTICR[0] &= ~(0xF << 0); //lllllllll PA0 source
    EXTI->IMR  |= (1 << 0);           // Unmask line 0
    EXTI->FTSR |= (1 << 0);           // Falling edge
    NVIC_EnableIRQ(EXTI0_IRQn);

    // Main Loop 
    while (1)
    {
        tempReading  = ADC_Read(2);  // LM35
        moistReading = ADC_Read(3);  // Moisture

        //  Decision Logic 
        if (tempReading < tempThreshold && moistReading > moistThreshold)
        {
            GPIOC->ODR |= (1U << 13); //(red light)
					  GPIOC->ODR &= ~(1U << 14);
            const char *msg = "condition : raining, dry sand -> No Irrigation\r\n";
            for (const char *p = msg; *p; p++)
            {
                while (!(USART1->SR & (1U << 7))) {}
                USART1->DR = *p;
            }
        }
        else if (tempReading < tempThreshold && moistReading <= moistThreshold)
        {
            GPIOC->ODR |= (1U << 13); //(red light)
					  GPIOC->ODR &= ~(1U << 14);
					const char *msg = "condition : raining, wet soil -> No Irrigation\r\n";
            for (const char *p = msg; *p; p++)
            {
                while (!(USART1->SR & (1U << 7))) {}
                USART1->DR = *p;
            }
        }
        else if (tempReading > tempThreshold && moistReading < moistThreshold)
        {
					  GPIOC->ODR |= (1U << 13); //(red light)
            GPIOC->ODR &= ~(1U << 14);
            const char *msg = "condition : not raining, wet soil -> No Irrigation\r\n";
            for (const char *p = msg; *p; p++)
            {
                while (!(USART1->SR & (1U << 7))) {}
                USART1->DR = *p;
            }
        }
        else if (tempReading > tempThreshold && moistReading > moistThreshold)
        {
            GPIOC->ODR |= (1U << 14); // (green light)
						GPIOC->ODR &= ~(1U << 13);
					const char *msg = "condition : not raining, dry soil -> Irrigating\r\n";
            for (const char *p = msg; *p; p++)
            {
                while (!(USART1->SR & (1U << 7))) {}
                USART1->DR = *p;
            }
        }
        delay_ms(2000);
    }
}

// EXTI0 Interrupt Handler
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1 << 0))
    {
        EXTI->PR |= (1 << 0); 

        const char *msg = "Irrigation Stopped\r\n";
        for (const char *p = msg; *p; p++)
        {
            while (!(USART1->SR & (1U << 7))) {}
            USART1->DR = *p;
        }
        GPIOC->ODR &= ~(1U << 14); // LED OFF when stopped
    }
}

// ADC READ FUNCTION 
uint16_t ADC_Read(uint8_t channel)
{
    ADC1->SQR3 = channel;
    ADC1->CR2 |= (1U << 30);
    while (!(ADC1->SR & (1U << 1))) {}
    return (uint16_t)ADC1->DR;
}