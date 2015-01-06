#ifndef __OW_H__
#define __OW_H__

#include <public.h>
#include "settings.h"
#include "hardware.h"

// USART1, USART2, ...
#ifndef OW_USART
#	error OW_USART must be defined
#endif
// APB1ENR, ...
#ifndef OW_USART_APB
#	error OW_USART_APB must be defined
#endif
// RCC_APB1ENR_USART2EN, ...
#ifndef OW_USART_APBEN
#	error OW_USART_APBEN must be defined
#endif

#define OW_SEARCH_ROM   0xf0
#define OW_READ_ROM     0x33
#define OW_MATCH_ROM    0x55
#define OW_SKIP_ROM     0xcc
#define OW_ALARM_SEARCH 0xec
#define OW_CONVERT_T    0x44
#define OW_WRITE_SCRATCHPAD 0x4e
#define OW_READ_SCRATCHPAD  0xbe
#define OW_COPY_SCRATCHPAD  0x48
#define OW_RECALL_E2        0xe8
#define OW_READ_SUPPLY      0xb4

static inline void OW_UART_init()
{
	RCC->OW_USART_APB |= OW_USART_APBEN;
}

#define OW_RESET_NOTPRESENT 0
#define OW_RESET_PRESENT    1

void OW_UART_set9600();
void OW_UART_set115200();
uint8_t OW_UART_resetPulse();
uint8_t OW_UART_readBit();
uint8_t OW_UART_writeBit(uint8_t b);
void OW_UART_writeByte(uint8_t byte);
uint8_t OW_UART_readByte();
uint8_t OW_crc8_update(uint8_t crc, char d);

#define OW_ROMSEARCH_ROM       0
#define OW_ROMSEARCH_CONTINUE  1
#define OW_ROMSEARCH_ROMEND    2
#define OW_ROMSEARCH_NOSENSORS 3

struct TOWUARTSearchBuffer
{
	uint8_t i, j;
	int8_t collisionIdx, prevCollisionIdx;
	uint8_t rom[8];
};
void OW_UART_romSearchInit(struct TOWUARTSearchBuffer* data);
uint8_t OW_UART_romSearch(struct TOWUARTSearchBuffer* data);

#endif
