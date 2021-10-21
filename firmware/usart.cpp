#include "usart.h"


ISR(USART0_DRE_vect) {
	if (uart.txIndex != uart.txSize)
		USART0.TXDATAL = uart.txBuffer[uart.txIndex++];
	else
		USART0.CTRLA &= ~USART_DREIE_bm;
}

ISR(USART0_TXC_vect) {
	USART0.CTRLA &= ~USART_TXCIE_bm;
}

ISR(USART0_RXC_vect) {
	if (uart.rxIndex != uart.rxSize) {
		uart.rxBuffer[uart.rxIndex] = USART0.RXDATAL;
		if (uart.rxBuffer[uart.rxIndex++] == uart.rxEOL)
			goto RX_COMPLETE;
	}
	else {
        RX_COMPLETE:
		USART0.CTRLA &= ~USART_RXCIE_bm;
	}
}

USART::USART(USART::BAUDRATE rate,
             USART::PINMUX pins) :
	txBuffer(0),
	txSize(0),
	txIndex(0) {
	PORTMUX.CTRLB |= (static_cast<uint8_t>(pins) << 0);
#ifdef __AVR_ATtiny402__
	if (pins == USART::PINMUX::NORMAL) {
		PORTA.OUTCLR = (1 << 6);
		PORTA.DIRSET = (1 << 6);	// Set PA1/Txd as output.
		PORTA.DIRCLR = (1 << 7);	// Set PA7/RxD as input.
	}
	else {
		PORTA.OUTCLR = (1 << 1);
		PORTA.DIRSET = (1 << 1);	// Set PA6/TxD as output
		PORTA.DIRCLR = (1 << 2);	// Set PA2/RxD as input.
	}
#endif
#ifdef __AVR_ATtiny814__
	if (pins == USART::PINMUX::NORMAL) {
		PORTB.OUTCLR = (1 << 2);
		PORTB.DIRSET = (1 << 2);	// Set PB2/Txd as output.
		PORTB.DIRCLR = (1 << 3);	// Set PB3/RxD as input.
	}
	else {
		PORTA.OUTCLR = (1 << 1);
		PORTA.DIRSET = (1 << 1);	// Set PA6/TxD as output
		PORTA.DIRCLR = (1 << 2);	// Set PA2/RxD as input.
	}
#endif
	USART0.BAUD = static_cast<uint16_t>(rate);

	USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc |
	               USART_CHSIZE_8BIT_gc |
	               USART_PMODE_DISABLED_gc |
	               USART_SBMODE_1BIT_gc;
	USART0.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
}

uint8_t USART::rxBusy() const {
	if (rxIndex != rxSize)
		return true;
	return false;
}

void USART::finishRX() const {
	while (rxBusy());
}

uint8_t USART::txBusy() const {
	if (txIndex != txSize)
		return true;
	return false;
}

void USART::finishTX() const {
	while (txBusy());
}

uint8_t USART::bytesRead() const {
	return rxIndex;
}

uint8_t USART::bytesWritten() const {
	return txIndex;
}

const USART& USART::operator<<(const uint8_t &src) const {
	USART0.TXDATAL = src;
	while (!(USART0.STATUS & USART_DREIF_bm));
	return *this;
}

void USART::operator<<(const char *str) {
	txBuffer = reinterpret_cast<const uint8_t *>(str);
	uint8_t size = 0;
	while (str[size++]);
	txSize = size;
	txIndex = 0;
	USART0.CTRLA |= USART_DREIE_bm | USART_TXCIE_bm;
}

const USART& USART::operator >>(uint8_t& dest) const {
	uint8_t __attribute__((unused)) discard = USART0.RXDATAL;
	while ((~USART0.STATUS) & USART_RXCIF_bm);
	dest = USART0.RXDATAL;
	return *this;
}

void USART::read(uint8_t *dest, const uint8_t n,
                 const char eol) {
	uint8_t __attribute__((unused)) discard = USART0.RXDATAL;
	if (n != 0) {
		rxBuffer = reinterpret_cast<uint8_t *>(dest);
		rxSize = n;
		rxIndex = 0;
		rxEOL = eol;
		USART0.STATUS = USART_RXCIF_bm;
		USART0.CTRLA |= USART_RXCIE_bm;
	}
}
