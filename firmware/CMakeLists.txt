cmake_minimum_required(VERSION 3.19)

project(AquariumDaglig
	LANGUAGES CXX
	VERSION 0.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_MCU attiny402)

set(SOURCES
	main.cpp
	fakkel.cpp fakkel.h
	usart.cpp usart.h
	adc.cpp adc.h
	)

add_executable(AquariumDaglig ${SOURCES})
add_custom_command(TARGET AquariumDaglig POST_BUILD
  COMMAND avr-objcopy -j .text -j .data -j .fuse -j .lock -j .rodata -O ihex $<TARGET_FILE:AquariumDaglig> $<TARGET_FILE:AquariumDaglig>.hex
  COMMAND avr-size -t $<TARGET_FILE_DIR:AquariumDaglig>/CMakeFiles/AquariumDaglig.dir/*)


target_compile_definitions(AquariumDaglig PUBLIC __AVR_ATtiny402__)
target_compile_definitions(AquariumDaglig PUBLIC __AVR_XMEGA__)
target_include_directories(AquariumDaglig PUBLIC "/home/liquidsquid/x-tools/avr/avr/include")
target_compile_options(AquariumDaglig PUBLIC -Wall -fno-exceptions -fno-rtti -O1 -mmcu=${TARGET_MCU})
target_link_options(AquariumDaglig PUBLIC -mmcu=${TARGET_MCU})
