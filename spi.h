/*
 * spi.h
 *
 *  Created on: Oct 29, 2019
 *      Author: blake
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_

#include "xspi.h"

static u32 status;
static u32 controlReg;

XSpi spi;
XSpi_Config *spiConfig;	/* Pointer to Configuration data */

void init_spi();

#endif /* SRC_SPI_H_ */
