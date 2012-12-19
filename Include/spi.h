#ifndef __spi_h_
#define __spi_h_

/* Put these in bits 3:0 of SSPCON1 (SSPM3 - SSPM0) */
#define SPI_FOSC_4    0x00  // SPI Master mode, clock = Fosc/4
#define SPI_FOSC_16   0x01  // SPI Master mode, clock = Fosc/16
#define SPI_FOSC_64   0x02  // SPI Master mode, clock = Fosc/64
#define SPI_FOSC_TMR2 0x03  // SPI Master mode, clock = TMR2 output/2
#define SLV_SSON      0x04  // SPI Slave mode, SS pin control enabled 
#define SLV_SSOFF     0x05  // SPI Slave mode, SS pin control disabled

/* Bit 7 of SSPSTAT */
#define SMPMID          0x00
#define SMPEND          0x80

/* Not mapped directly into registers! */
#define MODE_00         0x00    // CKE=1, CKP=0
#define MODE_01         0x01    // CKE=0, CKP=0
#define MODE_10         0x02    // CKE=1, CKP=1
#define MODE_11         0x03    // CKE=0, CKP=1

#endif

