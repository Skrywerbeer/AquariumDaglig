#include "fakkel.h"

#include <avr/interrupt.h>
#include <avr/cpufunc.h>

Fakkel fakkel;

ISR(TCB0_INT_vect) {
	TCB0.INTFLAGS = TCB_CAPTEI_bm;
	TCA0.SINGLE.CNT = 0;
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	if (AC0.STATUS & AC_STATE_bm) {
		EVSYS.ASYNCSTROBE = (1 << 0);
		fakkel._status.flags.inductorNotReset = 1;
		return;
	}
}

ISR(AC0_AC_vect) {
	AC0.STATUS = AC_CMP_bm;
	fakkel._ontime = TCA0.SINGLE.CNT;
}

Fakkel::Fakkel(const uint16_t time) {
	comp_init();
	TCA0_init();
	TCB0_init();
	ports_init();
	setDeadtime(time);
}

void Fakkel::setDeadtime(const uint16_t time) {
	_deadtime = time;
}

uint16_t Fakkel::deadtime() const {
	return _deadtime;
}

uint16_t Fakkel::ontime() const {
	return _ontime;
}

void Fakkel::setReference(VREF_DAC0REFSEL_enum ref) {
	VREF.CTRLA &= ~VREF_DAC0REFSEL_gm;
	VREF.CTRLA |= ref;
}

void Fakkel::setPowerlevel(const Powerlevel level) {
	_powerlevel = static_cast<uint8_t>(level);
	switch(level) {
		case (Powerlevel::Level1):
			setReference(VREF_DAC0REFSEL_0V55_gc);
			break;
		case (Powerlevel::Level2):
			setReference(VREF_DAC0REFSEL_1V1_gc);
			break;
		case (Powerlevel::Level3):
			setReference(VREF_DAC0REFSEL_1V5_gc);
			break;
		case (Powerlevel::Level4):
			setReference(VREF_DAC0REFSEL_2V5_gc);
			break;
		case (Powerlevel::Level5):
			setReference(VREF_DAC0REFSEL_4V34_gc);
			break;
		default:
			setReference(VREF_DAC0REFSEL_0V55_gc);
	}
}

Fakkel::Powerlevel Fakkel::powerlevel() const {
	return static_cast<Fakkel::Powerlevel>(_powerlevel);
}

void Fakkel::operator++() {
	if (++_powerlevel == 6)
		_powerlevel = 1;
	setPowerlevel(static_cast<Powerlevel>(_powerlevel));
}

void Fakkel::operator--() {
	if (--_powerlevel == 255)
		_powerlevel = 5;
	setPowerlevel(static_cast<Powerlevel>(_powerlevel));
}

uint8_t Fakkel::enabled() const {
	if (TCB0.CTRLB & TCB_CCMPEN_bm)
		return true;
	return false;
}

const FakkelStatusFlags &Fakkel::status() const{
	return _status.flags;
}

void Fakkel::clearStatus() {
	_status.field = 0;
}

void Fakkel::clearInductorNotResetStatus() {
	_status.flags.inductorNotReset = 0;
}

void Fakkel::clearUnderCurrentStatus() {
	_status.flags.underCurrent = 0;
}

void Fakkel::comp_init() const {
	// Select VREF as inverting input,
	// PIN 3 as non-inverting input.
	// Output high when current threshold reached.
	VREF.CTRLA &= ~VREF_DAC0REFSEL_gm;
	VREF.CTRLA |= VREF_DAC0REFSEL_0V55_gc | AC_INTMODE_POSEDGE_gc;
	AC0.MUXCTRLA = AC_MUXNEG_VREF_gc | AC_MUXPOS_PIN0_gc;
	AC0.INTCTRL = AC_CMP_bm;
	AC0.CTRLA |= AC_ENABLE_bm;
}

void Fakkel::TCA0_init() const {
	
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc |TCA_SINGLE_ENABLE_bm;
}

void Fakkel::TCB0_init() const {
	// Connect ASYNC event channel 0 to AC0 as generator
	// and TCB0 as the user.
	EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_AC0_OUT_gc;
	EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_ASYNCCH0_gc;
	TCB0.EVCTRL = TCB_CAPTEI_bm; // Start counter on positive edge.
	TCB0.CTRLA = TCB_RUNSTDBY_bm;
	// Configure TCB for single shot mode, set output asynchronously.
	TCB0.CTRLB = TCB_CNTMODE_SINGLE_gc |  TCB_ASYNC_bm;
	TCB0.INTCTRL = TCB_CAPTEI_bm;
}

void Fakkel::ports_init() const {
	// Setup switch control pin.
	// Drive output high to close, drive low to open.
	PORTA.DIRSET = PIN6_bm;
	PORTA.OUTSET = PIN6_bm;
	PORTA.PIN6CTRL = PORT_INVEN_bm;
	// Setup comparator pin.
	PORTA.DIRCLR = PIN7_bm;
}

void Fakkel::enable() const {
	AC0.CTRLA |= AC_ENABLE_bm;
	TCB0.CTRLB |= TCB_CCMPEN_bm;
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

void Fakkel::disable() const {
	AC0.CTRLA &= ~AC_ENABLE_bm;
	TCB0.CTRLB &= ~TCB_CCMPEN_bm;
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	PORTA.OUTSET = PIN6_bm;     // REMEMBER THE PIN IS INVERTED!
}

void Fakkel::toggle() const {
	if (TCB0.CTRLB & TCB_CCMPEN_bm)
		disable();
	else
		enable();
}
