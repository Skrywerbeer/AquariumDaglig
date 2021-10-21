// *****************************************************************************
// File: fakkel.h
// Written By: Johan Grobler
// *****************************************************************************
// Peripherals used TCA0 TCB0 AC0.
// TCA0 is used to measure the ripple period.
// TCB0 is used as a single shot timer to give constant off
// time for the mosfet.
// AC0 compares the inductor current to a voltage reference
// and starts TCB0 when is "trips".
// *****************************************************************************

#ifndef FAKKEL_H
#define FAKKEL_H

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TCB0_INT_vect);
ISR(AC0_AC_vect);

struct FakkelStatusFlags {
		uint8_t inductorNotReset : 1;
		uint8_t underCurrent : 1;
};

class Fakkel {
	public:
		enum class Powerlevel : uint8_t {
			Level1 = 1,
			Level2,
			Level3,
			Level4,
			Level5
		};
		Fakkel(const uint16_t time=4);
		void setDeadtime(const uint16_t time);
		uint16_t deadtime() const;
		uint16_t ontime() const;
		void setReference(VREF_DAC0REFSEL_enum ref);
		void setPowerlevel(const Powerlevel level);
		Powerlevel powerlevel() const;
		void operator++();
		void operator--();

		uint8_t enabled() const;
		const FakkelStatusFlags &status() const;
		// Is die nodig?
		void clearStatus();
		void clearInductorNotResetStatus();
		void clearUnderCurrentStatus();
		// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		void enable() const;
		void disable() const;
		void toggle() const;
	private:
		void comp_init() const;
		void TCA0_init() const;
		void TCB0_init() const;
		void ports_init() const;

		register16_t &_deadtime = TCB0.CCMP;
		volatile uint16_t _ontime = 0;
		uint8_t _powerlevel = 0;
		union {
				FakkelStatusFlags flags;
				uint8_t field;
		} _status;
		friend void TCB0_INT_vect();
		friend void AC0_AC_vect();
};



extern Fakkel fakkel;

#endif // FAKKEL_H
