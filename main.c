#include "stm32f4xx.h"

volatile uint32_t ticks = 0;
uint8_t contador = 0;
uint8_t velocidad = 1;
uint8_t pausa = 0;

void SysTick_Handler(void){
    ticks++;
}

void SysTick_Init(void){
    SysTick->LOAD = 16000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 7;
}

void GPIO_Init(void){
    RCC->AHB1ENR |= (1<<0) | (1<<1) | (1<<2);

    // GPIOA: PA0, PA1, PA4, PA10
    GPIOA->MODER &= ~(
        (3<<(0*2)) |
        (3<<(1*2)) |
        (3<<(4*2)) |
        (3<<(10*2))
    );

    GPIOA->MODER |= (
        (1<<(0*2)) |
        (1<<(1*2)) |
        (1<<(4*2)) |
        (1<<(10*2))
    );

    // GPIOB: PB3, PB4, PB5
    GPIOB->MODER &= ~(
        (3<<(3*2)) |
        (3<<(4*2)) |
        (3<<(5*2))
    );

    GPIOB->MODER |= (
        (1<<(3*2)) |
        (1<<(4*2)) |
        (1<<(5*2))
    );

    // Botones PB6 y PB7 entrada con pull-down
    GPIOB->MODER &= ~((3<<12)|(3<<14));
    GPIOB->PUPDR |= ((2<<12)|(2<<14));

    // PC13 entrada (pausa)
    GPIOC->MODER &= ~(3<<26);
}

void limpiar_display(){
    GPIOA->ODR &= ~((1<<0)|(1<<1)|(1<<4)|(1<<10));
    GPIOB->ODR &= ~((1<<3)|(1<<4)|(1<<5));
}

void actualizar_display(uint8_t num){

    limpiar_display();

    switch(num){

        case 0:
            GPIOA->ODR |= (1<<1)|(1<<0)|(1<<4)|(1<<10);
            GPIOB->ODR |= (1<<3)|(1<<5);
            break;

        case 1:
            GPIOA->ODR |= (1<<1);       // b
            GPIOB->ODR |= (1<<3);       // c
            break;

        case 2:
            GPIOA->ODR |= (1<<1)|(1<<0)|(1<<4);
            GPIOB->ODR |= (1<<5)|(1<<4);
            break;

        case 3:
            GPIOA->ODR |= (1<<1)|(1<<0);
            GPIOB->ODR |= (1<<3)|(1<<5)|(1<<4);
            break;

        case 4:
            GPIOA->ODR |= (1<<1)|(1<<10);
            GPIOB->ODR |= (1<<3)|(1<<4);
            break;

        case 5:
            GPIOA->ODR |= (1<<0)|(1<<10);
            GPIOB->ODR |= (1<<3)|(1<<5)|(1<<4);
            break;

        case 6:
            GPIOA->ODR |= (1<<0)|(1<<4)|(1<<10);
            GPIOB->ODR |= (1<<3)|(1<<5)|(1<<4);
            break;

        case 7:
            GPIOA->ODR |= (1<<1)|(1<<0);
            GPIOB->ODR |= (1<<3);
            break;
    }
}

void leer_botones(){

    static uint32_t last = 0;

    if(ticks - last < 200) return;

    // botón rápido (PB6)
    if(GPIOB->IDR & (1<<6)){
        if(velocidad < 3) velocidad++;
        last = ticks;
    }

    //  botón lento (PC7 - D9)
    if(GPIOC->IDR & (1<<7)){
        if(velocidad > 1) velocidad--;
        last = ticks;
    }

    // pausa
    if(!(GPIOC->IDR & (1<<13))){
        pausa = !pausa;
        last = ticks;
    }
}
uint32_t get_delay(){
    if(velocidad == 1) return 1500;
    if(velocidad == 2) return 800;
    return 300;
}

int main(void){
    GPIO_Init();
    SysTick_Init();

    uint32_t last = 0;
    contador = 0;

    while(1){

        leer_botones();

        if(!pausa){
            if((ticks - last) >= get_delay()){
                last = ticks;

                contador++;
                if(contador > 7) contador = 0;
            }
        }

        actualizar_display(contador);
    }
}
