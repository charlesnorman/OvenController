/*
 * uart.c
 *
 *  Created on: Feb 12, 2023
 *      Author: Charles
 */
#include <stdint.h>
#include "uart.h"

#define DBG_UART_BAUDRATE	115200
#define SYS_FREQ		16000000
#define APB1_CLK		SYS_FREQ

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
static void uart_write(int ch);

int __io_putchar(int ch)
{
	uart_write(ch);
	return ch;
}

void debug_uart_init(void)
{
	/*Enable clock access to UART2
	 * Configure UART baud rate
	 * Configure transfer direction
	 * Enable UART module*/

	/*Enable clock*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	/*Set mode of PA2.  Bit 2:0 reset, Bit 2:1 set*/
	GPIOA->MODER |= GPIO_MODER_MODE2_1;
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_0);
	/*Set alternate function type to AF7(UART2_TX)*/
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL2_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL2_1;
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL2_2;
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2_3);

	/*Configure UART baud rate*/
	uart_set_baudrate(APB1_CLK, DBG_UART_BAUDRATE);
	/*Configure transfer direction and clear all other bits*/
	USART2->CR1 = USART_CR1_TE;
	/*Enable UART module*/
	USART2->CR1 |= USART_CR1_UE;

}

static void uart_write(int ch)
{
	/*Wait for transmit date register is empty*/
	while (!(USART2->SR & USART_SR_TXE))
	{
	};
	/*Write the transmit data register (lower 8 bits)*/
	USART2->DR = (ch & 0xFF);
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk + (baudrate / 2U)) / baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	USART2->BRR = compute_uart_bd(periph_clk, baudrate);
}
