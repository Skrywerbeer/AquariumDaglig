#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>
#include <avr/interrupt.h>

class ADC {
	public:
		ADC();
		uint16_t newSample();
		uint16_t lastValue = 0;
};

extern ADC adc;

#endif // ADC_H_
