#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(USART0_DRE_vect);
ISR(USART0_TXC_vect);
ISR(USART0_RXC_vect);

inline constexpr uint16_t calcBaud(const uint32_t baud) {
	const uint32_t F_PER{static_cast<uint32_t>(20e6)};
	// TODO: support 2X clock mode.
	return static_cast<uint16_t>(4*(F_PER/baud));
}

class USART {
	public:
		enum class BAUDRATE : uint16_t {
			BAUD_110 = calcBaud(110),
			BAUD_300 = calcBaud(300),
			BAUD_600 = calcBaud(600),
			BAUD_1200 = calcBaud(1200),
			BAUD_2400 = calcBaud(2400),
			BAUD_4800 = calcBaud(4800),
			BAUD_9600 = calcBaud(9600),
			BAUD_14400 = calcBaud(14400),
			BAUD_19200 = calcBaud(19200),
			BAUD_38400 = calcBaud(38400),
			BAUD_57600 = calcBaud(57600),
			BAUD_115200 = calcBaud(115200),
			BAUD_128000 = calcBaud(128000),
			BAUD_256000 = calcBaud(256000)
		};
		enum class PINMUX : uint8_t {
			NORMAL = 0,
			ALTERNATE = 1
		};
		USART(BAUDRATE rate=BAUDRATE::BAUD_9600,
		      PINMUX pins=PINMUX::NORMAL);

		uint8_t rxBusy() const;
		void finishRX() const;
		uint8_t txBusy() const;
		void finishTX() const;
		uint8_t bytesRead() const;
		uint8_t bytesWritten() const;

		const USART& operator<<(const uint8_t &src) const;

		// Write n bytes starting from src[0].
		// little endian write, write LSB first.
		template<typename T>
		void write(const T *src, uint8_t n);

		// TODO: write(uint8_t *src, uint8_t *end)

		// Array writing method.
		// little endian write. LSB at the lowest address.
		template<typename T, uint8_t n>
		void operator<<(const T (&src)[n]);
		// Basic string write, writes upto \0
		void operator<<(const char *str);

		// Blocking single byte read.
		const USART &operator>>(uint8_t &dest) const;

		// Interrupt based read.
		void read(uint8_t *dest, const uint8_t n,
		          const char eol='\0');
	private:
		const uint8_t *txBuffer;
		uint8_t txSize;
		volatile uint8_t txIndex;

		uint8_t *rxBuffer;
		uint8_t rxSize;
		volatile uint8_t rxIndex;
		char rxEOL;

		friend void USART0_DRE_vect();
		friend void USART0_TXC_vect();
		friend void USART0_RXC_vect();

};

template<typename T, uint8_t n>
void USART::operator<<(const T (&src)[n]) {
	txBuffer = reinterpret_cast<const uint8_t *>(&src);
	txSize = n*sizeof(T);
	txIndex = 0;
	USART0.CTRLA |= USART_DREIE_bm | USART_TXCIE_bm;
}

template<typename T>
void USART::write(const T *src, uint8_t n) {
	if (n != 0) {
		const uint8_t *srcByte = reinterpret_cast<const uint8_t *>(src);
		txBuffer = srcByte;
		txSize = n;
		txIndex = 0;
		USART0.CTRLA |= USART_DREIE_bm | USART_TXCIE_bm;
	}
}

extern USART uart;
#endif	// USART_H_
