#include <stdio.h>
#include <stdint.h>

__sfr __at 0xb4 ADDR;
__sfr __at 0xb5 DATA;
__sfr __at 0xa9 PPIB;
__sfr __at 0xaa PPIC;

void main(void)
{
	uint8_t addr;
	uint8_t rtc[0xd];

	ADDR = 0xd;
	DATA = 0b1000;

	printf("\x1b\x78\x35");

	while (1) {
		for (addr = 0x0 ; addr < 0xd ; addr++) {
			ADDR = addr;
			rtc[addr] = DATA;

		}
		printf("%u%u/%u%u/%u%u %u%u:%u%u:%u%u\r",
				(rtc[0xc] & 0b1111) + 198, rtc[0xb] & 0b1111,
				rtc[0xa] & 0b0001, rtc[0x9] & 0b1111,
				rtc[0x8] & 0b0011, rtc[0x7] & 0b1111,
				rtc[0x5] & 0b0011, rtc[0x4] & 0b1111,
				rtc[0x3] & 0b0111, rtc[0x2] & 0b1111,
				rtc[0x1] & 0b0111, rtc[0x0] & 0b1111);
__asm
		di
__endasm
		PPIC = (PPIC & 0b11110000) | 0x7;
		if (!(PPIB & 0x4)) { /* ESC */
			break;
		}
	}

	return 0;
}
