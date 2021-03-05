Implementation "impl1"

Configured to write bit output file to SPI Flash @address 0x000000 in order to use boot image from SPI - cannot make work without dual boot
used as the working implementation (SPI Flash @address 0x010000 in order to use dual boot primary image from SPI)

Implementation "working"

Configured to write bit output file to SPI Flash @address 0x010000 in order to use dual boot primary image from SPI

Implementation "failsafe"

Configured to write jedec output file to internal NVCM memory in order to use dual boot failsafe image from NVCM

