#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

__sfr __at 0xb4 ADDR;
__sfr __at 0xb5 DATA;

#define PPM_PER_CLK (1000000. / 32768.)
#define PPM_PER_CAL (121. / 127.)

static void usage(void)
{
	fprintf(stderr, "Usage: %s <ppm>\n", "calrtc");
	exit(1);
}

int main(int argc, char *argv[])
{
	char *ptr;
	int32_t ppm;
	uint32_t prl;
	uint8_t cal;
	uint8_t i;

	if (argc != 2) {
		usage();
	}

	ppm = strtol(argv[1], &ptr, 10);
	if (*ptr) {
		usage();
	}
	if (labs(ppm) > 500000) {
		usage();
	}

	//printf("PPM %ld\n", ppm);

	if (ppm < 0) { /* 遅くする */
		ppm = labs(ppm);
		prl = 32767 + floor(ppm / PPM_PER_CLK);
		cal = round((ppm - (floor(ppm / PPM_PER_CLK) * PPM_PER_CLK)) / PPM_PER_CAL);
	} else { /* 速くする */
		prl = 32767 - ceil(ppm / PPM_PER_CLK);
		cal = round(((ceil(ppm / PPM_PER_CLK) * PPM_PER_CLK) - ppm) / PPM_PER_CAL);
	}

	printf("PRL:%lu CAL:%u\n", prl, cal);

	/* アドレスに0xe(テストレジスタ)を指定して任意のデータを連続128回書き込むと較正モードになる(途中でADDRが操作された場合は全操作キャンセル) */
	ADDR = 0xe;
	for (i = 0 ; i < 128 ; i++) {
		DATA = 0x0;
	}

	/* RTC_PRLH/RTC_PRLL(20ビット)を上位から4ビットずつ書込み */
	DATA = prl >> 16;
	DATA = prl >> 12;
	DATA = prl >> 8;
	DATA = prl >> 4;
	DATA = prl >> 0;

	/* BKP_RTCCRのCCO/CAL(8ビット)を上位から4ビットずつ書込み */
	//cal |= 0x80; /* CCO */
	DATA = cal >> 4;
	DATA = cal >> 0; /* ここで較正データRTCがに反映される */

	return 0;
}
