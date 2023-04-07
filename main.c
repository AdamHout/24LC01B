/*******************************************************************************
 * Program Overview:
 * Interface a PIC24F to a MCP24LC01B I2C EEPROM
 *******************************************************************************
 * MCU: PIC24F16KA102 FOSC = 32MHz FCY = 16MHz
 *******************************************************************************
 * Peripherals used:
 * I2C1 - Connection to the 24LC01B
 * *****************************************************************************
 * External devices:
 * MCP24LC01B 1kbit EEPROM
 * *****************************************************************************
 * Pointers:
 * This program exercises the lc01b library
 * *****************************************************************************
 * Revisions:
 * 
 * Date                        Comments
 * 02/23  Adam Hout            Original source
 ******************************************************************************/

// PIC24F16KA102 Configuration Bit Settings
// FBS
#pragma config BWRP = OFF                                                       // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                                                        // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF                                                       // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                                                        // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL                                                   // Oscillator Select (Fast RC oscillator with divide-by-N with PLL module (FRCDIV+PLL))
#pragma config IESO = OFF                                                       // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE                                                   // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON                                                    // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = HS                                                    // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP                                                 // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSDCMD                                                   // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS32768                                                  // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128                                                    // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF                                                     // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF                                                     // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3                                                     // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON                                                      // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI                                                    // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18                                                       // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON                                                       // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx1                                                       // ICD Pin Placement Select bits (PGC1/PGD1 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF                                               // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC                                                  // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC                                                    // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = OFF                                                    // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR disabled in Deep Sleep)
#pragma config DSWDTEN = OFF                                                    // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)

//Includes
#include "xc.h"
#include "sys.h"
#include "lc01b.h"
#include <libpic30.h>

#define LED_T TRISBbits.TRISB15
#define LED_L LATBbits.LATB15

void errHandler(void);

int main(void) {
   
   uint8_t ctr;
   uint8_t memByte = 0x00;
   uint8_t byteOut = 0x45;
   uint8_t byteIn;
   uint8_t dataOut[128];
   uint8_t *pPage = dataOut;
   uint8_t dataIn[128];
   
   ee_Errors_t errCode;                                                         //Enumerated error list
   
   
   LED_T = 0;                                                                   //Set pin as output
   LED_L = 1;                                                                   //Turn LED on
   
   //Enable I2C with a 100kHz clock
   init_I2C(I2C_BRG_100);
   
   //Write and read byte functions
   errCode = lc01b_WriteByte(memByte,byteOut); 
   if(errCode)                                                                  //Error?
       errHandler();
   else
      errCode = lc01b_ReadByte(memByte,&byteIn);                                //No.. read the byte back
   
   //Fill the EEPROM with the ASCII table via page writes
   for (ctr=0;ctr<128;ctr++){                                                   //Fill the output buffer
      dataOut[ctr] = ctr;                                                       //0x00 - 0x7F
   }
   
   for(ctr=0;ctr<LC01B_CAP/LC01B_PAGE;ctr++){                                   //Page writes to the EEPROM
      errCode = lc01b_WritePage(memByte,LC01B_PAGE,pPage);
      if(errCode)                                                               //Break on an error
          break;
      pPage += LC01B_PAGE;                                                      //Ref the next page to send
      memByte += LC01B_PAGE;                                                    //Bump the EEPROM address
   }
   
   //Read the entire ROM sequentially
   if(errCode)
       errHandler();
   else
      lc01b_ReadSeq(0x00,LC01B_CAP,dataIn);
   
   //Write and read a floating point number
   float pi = 3.14;
   float x;
   
   errCode = lc01b_WriteObject(0x00,sizeof(pi),&pi);                            //Write the float
   if(errCode)
       errHandler();
   else
      lc01b_ReadObject(0x00,sizeof(x),&x);                                      //Read the float back
   
   //Write/read a two byte unsigned integer
   uint16_t twoByte = 53207;
   uint16_t y;
   
   errCode = lc01b_WriteObject(0x10,sizeof(twoByte),&twoByte);
   if(errCode)
       errHandler();
   else
      lc01b_ReadObject(0x10,sizeof(y),&y);
   
   //Generate a bounds error
   errCode = lc01b_WritePage(0x7D,LC01B_PAGE,dataOut);
   
   if(errCode)
       errHandler();
   
   while(1);
   return 0;
}

//Pseudo error handler code
void errHandler(void){
    
    while(1){
        LED_L = ~LED_L;
        __delay_ms(250);
    }
}