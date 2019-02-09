#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void spi_setup(void);
void spi_transfer(uint8_t *tx, uint8_t *rx, uint32_t len);

#endif /* SPI_H_ */
