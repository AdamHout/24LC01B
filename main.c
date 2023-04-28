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
 * This program demonstrates the lc01b library
 * MCU has a 256 word (16-bit) internal EEPROM
 * *****************************************************************************
 * Revisions:
 * 
 * Date                        Comments
 * -----------------------------------------------------------------------------
 * 02/23  Adam Hout            Original source
 * -----------------------------------------------------------------------------
 * 04/23 Hout                  Add logic to copy the contents of the LC01B
 *                             into the PIC24F16KA102's internal EEPROM
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
#include "lc01b.h"                                                              //MCP24LC01B EEPROM library
#include "obeeprom.h"                                                           //PIC24F on board EEPROM library
#include <libpic30.h>
#include <string.h>

//Status LED pin defines
#define LED_T TRISBbits.TRISB15
#define LED_L LATBbits.LATB15
#define NBR_PAGES LC01B_CAP/LC01B_PAGE

//Function declarations
void errHandler(void);

//---------------------------------------------
// Begin mainline logic
//---------------------------------------------
int main(void) {
   
    //Variables for the demo logic
   uint8_t  ctr, idx;
   uint8_t  memByte = 0x00;
   uint8_t  byteOut = 0x45;
   uint8_t  byteIn;
   uint8_t  dataOut[LC01B_CAP];
   uint8_t  dataIn[LC01B_CAP];
   uint8_t  *pPage = dataOut;
   uint16_t wordIn; 
   uint16_t dataWords[NUM_WORDS];    
   uint64_t bigUn;
   
   //Enumerated error list
   ee_Errors_t errCode;                                                         
   
   
   //Init status LED
   LED_T = 0;                                                                   //Set pin as output
   LED_L = 1;                                                                   //Turn LED on
   
   //Enable I2C with a 100kHz clock
   init_I2C(I2C_BRG_100);
   
   //--------------------------------------------------
   //Begin MCP24LC01B demo logic
   //--------------------------------------------------
   //Write and read byte functions
   errCode = lc01b_WriteByte(memByte,byteOut); 
   if(errCode)                                                                  //Error?
       errHandler();
   else
      errCode = lc01b_ReadByte(memByte,&byteIn);                                //No.. read the byte back
   
   //Write and read a floating point number
   float pi = 3.14;
   float x;
   
   errCode = lc01b_WriteObject(0x00,sizeof(pi),&pi);                            //Write the float
   if(errCode)
       errHandler();
   else
      lc01b_ReadObject(0x00,sizeof(x),&x);                                      //Read the float back
   
   //Write/read a 64-bit unsigned integer
   bigUn = 1844674407370955161;
   
   errCode = lc01b_WriteObject(0x10,sizeof(bigUn),&bigUn);
   if(errCode)
       errHandler();
   else{
      bigUn = 0;
      lc01b_ReadObject(0x10,sizeof(bigUn),&bigUn);
   }
   
   //Fill the EEPROM with the ASCII table via page writes
   for (ctr=0;ctr<LC01B_CAP;ctr++){                                             //Fill the output buffer
      dataOut[ctr] = ctr;                                                       //0x00 - 0x7F
   }
   
   for(ctr=0;ctr<NBR_PAGES;ctr++){                                              //Page writes to the EEPROM
      errCode = lc01b_WritePage(memByte,LC01B_PAGE,pPage);
      if(errCode)                                                               //Break on an error
          break;
      pPage += LC01B_PAGE;                                                      //Ref the next page to send
      memByte += LC01B_PAGE;                                                    //Bump the EEPROM address
   }
   
   //Read the entire LC01B sequentially
   if(errCode)
       errHandler();
   else
      lc01b_ReadSeq(0x00,LC01B_CAP,dataIn);
   
   
   //---------------------------------------------
   //Begin PIC24F on-board EEPROM demo logic
   //---------------------------------------------
   //Write to the fist and last EEPROM words
   obee_Write(EE_WRITE_ER,OFFSET_ZERO,0xDEAD);
   obee_Write(EE_WRITE_ER,OFFSET_LAST,0xDEAD);
   
   //Read back the first and last EEPROM words
   wordIn = obee_Read(OFFSET_ZERO);
   wordIn = obee_Read(OFFSET_LAST);
   
   //Copy 24LC01B contents into the PIC24F on-board EEPROM   
   for(ctr=0; ctr<LC01B_CAP; ctr+=WORD_LEN){
      memcpy(&wordIn,&dataIn[ctr],WORD_LEN);
      obee_Write(EE_WRITE_ER,ctr,wordIn);
   }
   
   //Read the contents back
   idx = 0;
   for(ctr=0; ctr<LC01B_CAP; ctr+=WORD_LEN){
      dataWords[idx++] = obee_Read(ctr);
   }
    
   //Erase words 12-15; 0x7FFE16 - 0x7FFE1C
   obee_Erase(EE_ERASE_FOUR,24);                                                //24 byte offset
   
   //Read the 1st 64 words (128 bytes) of the EEPROM sequentially
   obee_ReadSeq(0,128,dataWords);
   
   //Bulk erase the entire contents of the on-board EEPROM
   obee_Erase(EE_ERASE_BULK,0);
   
   //Fill the EEPROM contents with 0xA5A5
   //Using program only bit (NVMCONbits.pgmonly). Memory already erased
   memset(dataWords,0xA5A5,512);
   obee_WriteSeq(EE_WRITE_NOE,OFFSET_ZERO,512,dataWords);                       //No erase
   
   //Read back the contents
   memset(dataWords,0x0000,512);
   obee_ReadSeq(OFFSET_ZERO,512,dataWords);
   
   //Demo complete
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