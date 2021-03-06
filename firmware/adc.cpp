#include "adc.h"

ADC adc;

ADC::ADC() {
	ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc;
	ADC0.CTRLC = ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV16_gc;
	ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
	ADC0.CTRLA = ADC_ENABLE_bm;
}

uint16_t ADC::newSample() {
	ADC0.COMMAND = ADC_STCONV_bm;
	while (!ADC0.INTFLAGS & ADC_RESRDY_bm);
	adc.lastValue = (ADC0.RES >> 4);
	return adc.lastValue;
}
