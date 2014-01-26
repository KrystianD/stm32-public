#ifndef __KDUSB_LIB_H__
#define __KDUSB_LIB_H__

#include <stdint.h>

#include <kdusb_lib.h>

typedef union
{
	uint8_t bytes[2];
	uint16_t word;
} USBWord;

typedef struct
{
	uint8_t bmRequestType, bRequest;
	USBWord wValue;
	USBWord wIndex;
	uint16_t wLength;
} USB_Request;

uint8_t usbEP;
extern uint8_t *usbData;
extern USB_Request usbRequest;

void usbHandleLP ();

uint16_t usbFunctionSetup ();
uint8_t usbPrepareUserData (uint16_t max);
void usbHandleData (uint8_t size);

#endif
