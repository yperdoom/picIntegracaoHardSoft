/*
 * File:   main.c
 * Author: Prof. Emiliano A. Veiga
 *         emiliano.veiga@unoesc.edu.br
 * Copyright: Universidade do Oeste de Santa Catarina - UNOESC
 * Created on 21 de Setembro de 2017, 21:10
 */


#include <xc.h>
#include "config.h"
#include <plib/usart.h> /* instalar a plib para usar o PIC18F4550 do devKit*/
#include <adc.h>
#include <stdio.h>

/* modulo UART*/
void InitSerial();
void TransmiteByte(unsigned char byte);
unsigned char RecebeByte();

/* modulo conversor analogico digital
 * Inicializar modulo
 * Rotina para converter dado analogico conectado ao Canal 0
 * Utilizar quando for necessário ler a porta analogica
 */
void InitADCON();
unsigned char ConverterCanal0();

/*------------------------------------------------------------------------------*/

/*
 * Variáveis globais
 */
unsigned int presencaDetectada = 0;
/*------------------------------------------------------------------------------*/

/*
 * O codigo abaixo eh um exemplo de leitura de bytes recebidos na serial
 * e aciona rotinas e leitura do canal analogico e transferencia do dado
 * amostrado.
 *
 * Importante: Duvidas consultar o manual do devKit e o datasheet do microcontrolador PIC18F4550
 */
void main(void) {

    InitPorts(); /* define se as portas de IO sao entrada ou saida para cada bloco de registrador (Ports A, B, C...) */
    InitSerial(); /* deixa o modulo serial pronto para comunicacao. Observe a pinagem rx-tx correta */
    InitADCON(); /* deixa o modelo ADCON pronto para converter sinal analogico para digital */

//    unsigned char dadoDigital = 0x01;
    unsigned char received = 0x00;
//    unsigned char valorSensor;
    
    int lr = 0, ly = 0, lg = 0;
//    int br = 0, by = 0, bg = 0;

    for(;;) 
    {
        if(lr == 1){ LATBbits.LATB0 = 1; } else { LATBbits.LATB0 = 0; }
        if(ly == 1){ LATBbits.LATB1 = 1; } else { LATBbits.LATB1 = 0; }
        if(lg == 1){ LATBbits.LATB2 = 1; } else { LATBbits.LATB2 = 0; }
        
        if(PORTBbits.RB3 == 1)
            TransmiteByte('R');
        
        else if(PORTBbits.RB6 == 1)
            TransmiteByte('Y');
        
        else if(PORTBbits.RB7 == 1)
            TransmiteByte('G');
        
        
        TransmiteByte('\n');
       
        received = RecebeByte();        

        if(received == 'A'){ lr = 0; ly = 0; lg = 0; }
         if(received == 'B'){ lr = 1; ly = 0; lg = 0; }
        if(received == 'C'){ lr = 0; ly = 1; lg = 0; }
        if(received == 'D'){ lr = 1; ly = 1; lg = 0; }
        if(received == 'E'){ lr = 0; ly = 0; lg = 1; }
        if(received == 'F'){ lr = 1; ly = 0; lg = 1; }
        if(received == 'G'){ lr = 0; ly = 1; lg = 1; }
        if(received == 'H'){ lr = 1; ly = 1; lg = 1; } 
        

    }

    return;
}

/*------------------------------------------------------------------------------*/

void InitSerial() {
    OpenUSART(USART_TX_INT_OFF
            & USART_RX_INT_OFF
            & USART_ASYNCH_MODE
            & USART_EIGHT_BIT
            & USART_CONT_RX                                                            
            & USART_BRGH_HIGH, 155);
    //64 = 19200 com CLK = 20MHZ
    //155 = 19200 com CLK = 48 Mhz
}

/*------------------------------------------------------------------------------*/

void TransmiteByte(unsigned char byte) {
    while (!TXSTA1bits.TRMT); /* aguarda liberar modulo para envio de dado */
    putcUSART(byte);
}

/*------------------------------------------------------------------------------*/

unsigned char RecebeByte() {
    unsigned char b = 0x00;

    //check if the interrupt is caused by RX pin  
    b = getcUSART();

    return b;
}

/*------------------------------------------------------------------------------*/

void InitADCON() {
    ADCON0 = 0b00000001; /* habilitar modulo AD */

    /* no AN1  esta o LM35 do kit ACEPic e o AN0 esta o potenciometro*/
    ADCON1 = 0b00001101; /* canais 0 e 1 habilitados para leitura analógica AN0 e AN1 */

    ADCON2 = 0b00001000; /* esquerda justfif; Tacq=2Tad, Tad=2*Tosc ou Fosc/2 */
}

/*------------------------------------------------------------------------------*/

unsigned char ConverterCanal0() {
    unsigned int valorDigital;

    ADCON0 = 0b00000101; /* selecionar canal a ser amostrado = AN1 */

    ADCON0bits.GO_DONE = 1; /* solicitar amostragem */

    while (ADCON0bits.NOT_DONE); /* aguardar processamento */

    valorDigital = ADRESH; /* ler 8 bits MSB */
    
    if (valorDigital >= 40 && presencaDetectada == 0) {
        TransmiteByte(0x69);
        TransmiteByte(0x70);
        TransmiteByte(0x31);
        TransmiteByte(0x10);
        
        presencaDetectada = 1;
    }
    if (valorDigital < 40 && presencaDetectada == 1) {
        TransmiteByte(0x69);
        TransmiteByte(0x70);
        TransmiteByte(0x30);
        TransmiteByte(0x10);
        
        presencaDetectada = 0;
    }

    __delay_ms(50);
    
    return valorDigital;
}