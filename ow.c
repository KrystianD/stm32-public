#include "ow.h"
#include <delay.h>
#include <myprintf.h>

void OW_UART_set9600()
{
	IO_ALT_OPEN_DRAIN(OW);
	IO_HIGH(OW);
	OW_USART->BRR = USART_BRR(9600);
	OW_USART->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	OW_USART->CR3 = USART_CR3_HDSEL;
	uint8_t v = USART2->DR;
}
void OW_UART_set115200()
{
	IO_ALT_OPEN_DRAIN(OW);
	IO_HIGH(OW);
	OW_USART->BRR = USART_BRR(115200);
	OW_USART->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	OW_USART->CR3 = USART_CR3_HDSEL;
	uint8_t v = USART2->DR;
}
uint8_t OW_UART_resetPulse()
{
	uint8_t t = 0;
	OW_USART->DR = 0xf0;
	while (!(OW_USART->SR & USART_SR_RXNE))
	{
		t++;
		if (t == 100)
			return 2;
		_delay_us(200);
	}
	uint8_t v = OW_USART->DR;
	return v != 0xf0 ? OW_RESET_PRESENT : OW_RESET_NOTPRESENT;
}
uint8_t OW_UART_readBit()
{
	uint8_t t = 0;
	OW_USART->DR = 0xff;
	while (!(OW_USART->SR & USART_SR_RXNE))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us(2);
	}
	uint8_t v = OW_USART->DR;
	return v == 0xff ? 1 : 0;
}
uint8_t OW_UART_writeBit(uint8_t b)
{
	uint8_t t = 0;
	OW_USART->DR = b ? 0xff : 0x00;
	while (!(OW_USART->SR & USART_SR_RXNE))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us(2);
	}
	uint8_t v = OW_USART->DR;
}
void OW_UART_writeByte(uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OW_UART_writeBit(byte & 0x01);
		byte >>= 1;
	}
}
uint8_t OW_UART_readByte()
{
	uint8_t i, v = 0;
	for (i = 0; i < 8; i++)
	{
		v >>= 1;
		if (OW_UART_readBit())
			v |= 0x80;
	}
	return v;
}
uint8_t OW_crc8_update(uint8_t crc, char d)
{
	uint8_t j;
	
	crc ^= d;
	for (j = 0; j < 8; j++)
	{
		if (crc & 0x1)
		{
			crc >>= 1;
			crc ^= 0x8c;
		}
		else
		{
			crc >>= 1;
		}
	}
	return crc;
}

void OW_UART_romSearchInit(struct TOWUARTSearchBuffer* data)
{
	data->i = 0;
	data->j = 0;
	data->collisionIdx = -1;
	data->prevCollisionIdx = -1;

	OW_UART_set9600();
	OW_UART_resetPulse();
	OW_UART_set115200();
	OW_UART_writeByte(OW_SEARCH_ROM);
}
uint8_t OW_UART_romSearch(struct TOWUARTSearchBuffer* data)
{
	uint8_t destBit;
	uint8_t b = OW_UART_readBit();
	uint8_t bc = OW_UART_readBit();
	// myprintf("b: %d bc: %d\r\n", b, bc);
	
	uint8_t idx = data->i * 8 + data->j;
	
	if (b == 0 && bc == 0)
	{
		if (idx > data->collisionIdx)
		{
			data->collisionIdx = idx;
			destBit = 0;
		}
		else if (idx < data->collisionIdx)
		{
			destBit = (data->rom[data->i] & (1 << data->j)) ? 1 : 0;
		}
		else
		{
			destBit = 1;
			if (data->collisionIdx != data->prevCollisionIdx)
				data->collisionIdx = data->prevCollisionIdx;
			else
				data->collisionIdx = -1;
			data->prevCollisionIdx = -1;
		}
		if (destBit == 0)
		{
			data->prevCollisionIdx = idx;
		}
	}
	else if (b == 1 && bc == 1)
	{
		return OW_ROMSEARCH_NOSENSORS;
	}
	else
	{
		destBit = b;
	}
	// myprintf("go: %d i: %d j: %d idx: %d c: %d pc: %d", destBit, data->i, data->j, idx, data->collisionIdx, data->prevCollisionIdx);
	OW_UART_writeBit(destBit);
	if (destBit)
		data->rom[data->i] |= (1 << data->j);
	else
		data->rom[data->i] &= ~(1 << data->j);
		
	// myprintf("\n");
	
	data->j++;
	if (data->j == 8)
	{
		data->i++;
		data->j = 0;
		if (data->i == 8)
		{
			data->i = data->j = 0;
			if (data->collisionIdx == -1)
			{
				return OW_ROMSEARCH_ROMEND;
			}
			else
			{
				OW_UART_set9600();
				OW_UART_resetPulse();
				OW_UART_set115200();
				OW_UART_writeByte(OW_SEARCH_ROM);
				return OW_ROMSEARCH_ROM;
			}
		}
	}
	return OW_ROMSEARCH_CONTINUE;
}
