#include <avr/io.h>
#include <avr/fuse.h>
#define F_CPU 20e6
#include <util/delay.h>
#include <avr/interrupt.h>

#include "fakkel.h"
#include "usart.h"
#include "adc.h"

#include <stdlib.h>
#include <string.h>

FUSES{
	.WDTCFG = 0,
		.BODCFG = ACTIVE_ENABLED_gc | BOD_LVL_BODLEVEL7_gc,
		.OSCCFG = FREQSEL_20MHZ_gc,
		0xff,
		0x00,
		.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_UPDI_gc |
		FUSE_EESAVE_bm,
		.SYSCFG1 = SUT_64MS_gc,
		.APPEND = 0x00,
		.BOOTEND = 0x00,
};

//Fakkel fakkel;
USART uart(USART::BAUDRATE::BAUD_9600, USART::PINMUX::ALTERNATE);

void testCurrents() {
	fakkel.disable();
	fakkel.setOffTime(100);
	fakkel.setPowerlevel(Fakkel::Powerlevel::Level1);
	fakkel.enable();
	uart << "Input voltages\n\r"; uart.finishTX();
	for (uint8_t i = 0; i < 5; ++i) {
		uart << "level "; uart.finishTX();
		char i_str[2];
		utoa(i, i_str, 10);
		uart << i_str; uart.finishTX();
		uart << " : "; uart.finishTX();
		char voltage[5];
		utoa(adc.newSample(), voltage, 10);
		uart << (const char *)voltage; uart.finishTX();
		uart << "\n\r"; uart.finishTX();
		++fakkel;
		_delay_ms(50);
	}
	fakkel.disable();
}

void testChargeTimes() {
	char time[5];
	fakkel.disable();
	fakkel.setOffTime(1000);
	fakkel.setPowerlevel(Fakkel::Powerlevel::Level1);
	fakkel.enable();
	uart << "Charge times\n\r"; uart.finishTX();
	for (uint8_t i = 0; i < 5; ++i) {
		uart << "level "; uart.finishTX();
		char i_str[2];
		utoa(i, i_str, 10);
		uart << i_str; uart.finishTX();
		uart << " : "; uart.finishTX();
		utoa(fakkel.maxOnTime(), time, 10);
		uart << (const char *)time;
		uart.finishTX();
		uart << "\n\r";
		++fakkel;
		_delay_ms(50);
	}
	fakkel.disable();
}

constexpr uint16_t voltageToTicks(const double voltage) {
	return (voltage*1024.0)/(55.0);
}

int main() {
	CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = 0;
	sei();
	fakkel.setOffTime(80);
	fakkel.setPowerlevel(Fakkel::Powerlevel::Level1);
	while (adc.newSample() < voltageToTicks(14.0))
	fakkel.enable();
	while (1) {
		// testCurrents();
		// testChargeTimes();
		// _delay_ms(1000);
		adc.newSample();
		if ((adc.lastValue > voltageToTicks(18.5)) &&
		    (fakkel.powerlevel() != Fakkel::Powerlevel::Level5)) {
			++fakkel;
		}
		else if ((adc.lastValue < voltageToTicks(16.0)) &&
		         (fakkel.powerlevel() != Fakkel::Powerlevel::Level1)){
			--fakkel;
		}
		uart << "current voltage: ";
		char num[5];
		utoa(adc.lastValue*(55000/1024), num, 10);
		uart.finishTX();
		uart << (const char *)num; uart.finishTX();
		uart << " | power level: ";
		utoa(static_cast<uint8_t>(fakkel.powerlevel()), num, 10);
		uart.finishTX();
		uart << (const char *)num; uart.finishTX();
		uart << "\n\r";
		_delay_ms(1000);
	}
}
