
/* 
 * File:   config.h
 * Author: Emiliano
 * Comments: Configuracoes basicas do PIC18F4550
 * Revision history: 
 * 
 * Aqui sao definidas configuracoes e recursos basicos do MCU que sao
 * necessarios para o projeto.
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CONFIG_H
#define	CONFIG_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define _XTAL_FREQ  48000000 /* conferir a frequencia utilizada no seu kit */

#define SENSOR LATA0

#define ADC_START_CONVERSION (ADCON0 |= 0x04)
#define ADC_DONE (~ADCON0 & 0x04)
#define SENSOR_THRESHOLD 800

void InitPorts();
void delay_ms(int tempo);

#endif