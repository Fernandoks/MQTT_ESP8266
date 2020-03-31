/*
 * UartRingbuffer.c
 *
 *  Created on: 10-Jul-2019
 *      Author: Controllerstech
 */

#include "UartRingbuffer_multi.h"
#include <string.h>

/* define the UART you are using  */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


/*  Define the device uart and pc uart below according to your setup  */
#define device_uart &huart1
#define pc_uart &huart2

/* put the following in the ISR 

extern void Uart_isr (UART_HandleTypeDef *huart);

*/

/**************** no chnages after this **********************/


ring_buffer rx_buffer1 = { { 0 }, 0, 0};
ring_buffer tx_buffer1 = { { 0 }, 0, 0};
ring_buffer rx_buffer2 = { { 0 }, 0, 0};
ring_buffer tx_buffer2 = { { 0 }, 0, 0};

ring_buffer *_rx_buffer1;
ring_buffer *_tx_buffer1;
ring_buffer *_rx_buffer2;
ring_buffer *_tx_buffer2;

void store_char (unsigned char c, ring_buffer *buffer);


void Ringbuf_init(void)
{
  _rx_buffer1 = &rx_buffer1;
  _tx_buffer1 = &tx_buffer1;
  _rx_buffer2 = &rx_buffer2;
  _tx_buffer2 = &tx_buffer2;

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_ENABLE_IT(device_uart, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(pc_uart, UART_IT_ERR);

  /* Enable the UART Data Register not empty Interrupt */
  __HAL_UART_ENABLE_IT(device_uart, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(pc_uart, UART_IT_RXNE);
}

void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if(i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}

int Uart_read(UART_HandleTypeDef *uart)
{
	if (uart == device_uart)
	{
		  // if the head isn't ahead of the tail, we don't have any characters
		  if(_rx_buffer1->head == _rx_buffer1->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
		    _rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		  }
	}

	else if (uart == pc_uart)
	{
		  // if the head isn't ahead of the tail, we don't have any characters
		  if(_rx_buffer2->head == _rx_buffer2->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
		    _rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		  }
	}

	else return -1;
}

void Uart_write(int c, UART_HandleTypeDef *uart)
{
	if (c>0)
	{
		if (uart == device_uart){
		int i = (_tx_buffer1->head + 1) % UART_BUFFER_SIZE;

		// If the output buffer is full, there's nothing for it other than to
		// wait for the interrupt handler to empty it a bit
		// ???: return 0 here instead?
		while (i == _tx_buffer1->tail);

		_tx_buffer1->buffer[_tx_buffer1->head] = (uint8_t)c;
		_tx_buffer1->head = i;

		__HAL_UART_ENABLE_IT(device_uart, UART_IT_TXE); // Enable UART transmission interrupt
		}

		else if (uart == pc_uart){
			int i = (_tx_buffer2->head + 1) % UART_BUFFER_SIZE;

			// If the output buffer is full, there's nothing for it other than to
			// wait for the interrupt handler to empty it a bit
			// ???: return 0 here instead?
			while (i == _tx_buffer2->tail);

			_tx_buffer2->buffer[_tx_buffer2->head] = (uint8_t)c;
			_tx_buffer2->head = i;

			__HAL_UART_ENABLE_IT(pc_uart, UART_IT_TXE); // Enable UART transmission interrupt
			}
	}
}

int IsDataAvailable(UART_HandleTypeDef *uart)
{
	if (uart == device_uart) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail) % UART_BUFFER_SIZE;
	else if (uart == pc_uart) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;
	return -1;
}

uint16_t Get_position (char *string, UART_HandleTypeDef *uart)
{
  static uint8_t so_far;
  uint16_t counter;
  int len = strlen (string);
  if (uart == device_uart)
  {
	 if (_rx_buffer1->tail>_rx_buffer1->head)
	 {
	  if (Uart_read(device_uart) == string[so_far])
	  		{
	  		  counter=UART_BUFFER_SIZE-1;
	  		  so_far++;
	  		}
	  else so_far=0;
	 }
	 unsigned int start = _rx_buffer1->tail;
	 unsigned int end = _rx_buffer1->head;
	 for (unsigned int i=start; i<end; i++)
	 {
	  if (Uart_read(device_uart) == string[so_far])
		{
		  counter=i;
		  so_far++;
		}
	  else so_far =0;
	 }
  }
  else if (uart == pc_uart)
  {
	 if (_rx_buffer2->tail>_rx_buffer2->head)
	 {
	  if (Uart_read(pc_uart) == string[so_far])
	  		{
	  		  counter=UART_BUFFER_SIZE-1;
	  		  so_far++;
	  		}
	  else so_far=0;
	 }
	 unsigned int start = _rx_buffer2->tail;
	 unsigned int end = _rx_buffer2->head;
	 for (unsigned int i=start; i<end; i++)
	 {
	  if (Uart_read(pc_uart) == string[so_far])
		{
		  counter=i;
		  so_far++;
		}
	  else so_far =0;
	 }
  }

	 if (so_far == len)
	 {
	  so_far =0;
	  return counter;
	 }
	 else return -1;
}

int Get_after (char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart)
{
  uint16_t counter;

  counter = Get_position(string, uart);
  if (counter <= UART_BUFFER_SIZE)
  {
//	Uart_printbase(counter, 10, pc_uart);

	if (uart == device_uart)
	{
		for (int i=0; i<numberofchars; i++)
		{
			counter++;
			if (counter == UART_BUFFER_SIZE) counter = 0;
			while (!(IsDataAvailable(device_uart)));
			buffertosave[i] = Uart_read(device_uart);
		}

		return 1;
	}
	else if (uart == pc_uart)
	{
		for (int i=0; i<numberofchars; i++)
		{
			counter++;
			if (counter == UART_BUFFER_SIZE) counter = 0;
			while (!(IsDataAvailable(pc_uart)));
			buffertosave[i] = _rx_buffer2->buffer[counter];
		}

		return 1;
	}

	return 0;
  }
  	  else return 0;
}

void Uart_sendstring (const char *s, UART_HandleTypeDef *uart)
{
	while(*s!='\0') Uart_write(*s++, uart);
}

void Uart_printbase (long n, uint8_t base, UART_HandleTypeDef *uart)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];

  *s = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--s = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  while(*s) Uart_write(*s++, uart);
}

void Get_string (char *buffer, UART_HandleTypeDef *uart)
{
	int index=0;

	if (uart == device_uart)
	{
		while (_rx_buffer1->tail>_rx_buffer1->head)
		{
			if ((_rx_buffer1->buffer[_rx_buffer1->head-1] == '\n')||((_rx_buffer1->head == 0) && (_rx_buffer1->buffer[UART_BUFFER_SIZE-1] == '\n')))
				{
					buffer[index] = Uart_read(device_uart);
					index++;
				}
		}
		unsigned int start = _rx_buffer1->tail;
		unsigned int end = (_rx_buffer1->head);
		if ((_rx_buffer1->buffer[end-1] == '\n'))
		{

			for (unsigned int i=start; i<end; i++)
			{
				buffer[index] = Uart_read(device_uart);
				index++;
			}
	    }
	}

	if (uart == pc_uart)
	{
		while (_rx_buffer2->tail>_rx_buffer2->head)
		{
			if ((_rx_buffer2->buffer[_rx_buffer2->head-1] == '\n')||((_rx_buffer2->head == 0) && (_rx_buffer2->buffer[UART_BUFFER_SIZE-1] == '\n')))
				{
					buffer[index] = Uart_read(pc_uart);
					index++;
				}
		}
		unsigned int start = _rx_buffer2->tail;
		unsigned int end = (_rx_buffer2->head);
		if ((_rx_buffer2->buffer[end-1] == '\n'))
		{

			for (unsigned int i=start; i<end; i++)
			{
				buffer[index] = Uart_read(pc_uart);
				index++;
			}
	    }
	}
}


void Uart_isr (UART_HandleTypeDef *huart)
{
	  uint32_t isrflags   = READ_REG(huart->Instance->SR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);

    /* if DR is not empty and the Rx Int is enabled */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
    	 /******************
    	    	      *  @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
    	    	      *          error) and IDLE (Idle line detected) flags are cleared by software
    	    	      *          sequence: a read operation to USART_SR register followed by a read
    	    	      *          operation to USART_DR register.
    	    	      * @note   RXNE flag can be also cleared by a read to the USART_DR register.
    	    	      * @note   TC flag can be also cleared by software sequence: a read operation to
    	    	      *          USART_SR register followed by a write operation to USART_DR register.
    	    	      * @note   TXE flag is cleared only by a write to the USART_DR register.

    	 *********************/
		huart->Instance->SR;                       /* Read status register */
        unsigned char c = huart->Instance->DR;     /* Read data register */
        if (huart == device_uart)
        {
        	store_char (c, _rx_buffer1);  // store data in buffer
        }

        else if (huart == pc_uart)
        {
           	store_char (c, _rx_buffer2);  // store data in buffer
        }

        return;
    }

    /*If interrupt is caused due to Transmit Data Register Empty */
    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
    	if (huart == device_uart){
    	if(tx_buffer1.head == tx_buffer1.tail)
    	    {
    	      // Buffer empty, so disable interrupts
    	      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

    	    }

    	 else
    	    {
    	      // There is more data in the output buffer. Send the next byte
    	      unsigned char c = tx_buffer1.buffer[tx_buffer1.tail];
    	      tx_buffer1.tail = (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;

    	      /******************
    	      *  @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
    	      *          error) and IDLE (Idle line detected) flags are cleared by software
    	      *          sequence: a read operation to USART_SR register followed by a read
    	      *          operation to USART_DR register.
    	      * @note   RXNE flag can be also cleared by a read to the USART_DR register.
    	      * @note   TC flag can be also cleared by software sequence: a read operation to
    	      *          USART_SR register followed by a write operation to USART_DR register.
    	      * @note   TXE flag is cleared only by a write to the USART_DR register.

    	      *********************/

    	      huart->Instance->SR;
    	      huart->Instance->DR = c;

    	    }
    	}

    	else if (huart == pc_uart){
        	if(tx_buffer2.head == tx_buffer2.tail)
        	    {
        	      // Buffer empty, so disable interrupts
        	      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

        	    }

        	 else
        	    {
        	      // There is more data in the output buffer. Send the next byte
        	      unsigned char c = tx_buffer2.buffer[tx_buffer2.tail];
        	      tx_buffer2.tail = (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;

        	      /******************
        	      *  @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
        	      *          error) and IDLE (Idle line detected) flags are cleared by software
        	      *          sequence: a read operation to USART_SR register followed by a read
        	      *          operation to USART_DR register.
        	      * @note   RXNE flag can be also cleared by a read to the USART_DR register.
        	      * @note   TC flag can be also cleared by software sequence: a read operation to
        	      *          USART_SR register followed by a write operation to USART_DR register.
        	      * @note   TXE flag is cleared only by a write to the USART_DR register.

        	      *********************/

        	      huart->Instance->SR;
        	      huart->Instance->DR = c;

        	    }
        	}
    	return;
    }
}
