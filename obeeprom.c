/* Function library to access the on-board EEPROM of PIC24F 'K' family MCU's
 * 
 * Summary:
 *  The on-board EEPROM:
 *  - Consists of 256 16-bit words 0x7FFE00 - 0x7FFFFF
 *  - Can be accessed via PSV or __builtin table functions. Using the latter
 *  - Must be unlocked via an NVMKEY sequence prior to writing/erasing
 *  - A "program only" option (NVMCONbits.pgmonly) can be used on memory
 *    locations that have already been erased
 *  - Write/Erase operations do not impede normal program execution 
 *    (if using interrupts)
 */

#include "xc.h"
#include "obeeprom.h"

uint16_t __attribute__ ((space(eedata))) eedata;

//Erase 1, 4 or 8 EEPROM words or bulk erase the entire contents
void obee_Erase(uint16_t progOp, uint16_t offset){
    
    uint16_t ee_offset;
    
    //One, four, or eight word OR bulk erase
    NVMCON = progOp;                                          
    
    //Compute address if not a bulk erase
    if(progOp != EE_ERASE_BULK){                                             
        TBLPAG = __builtin_tblpage(&eedata);
        ee_offset = __builtin_tbloffset(&eedata) + offset; 
        __builtin_tblwtl(ee_offset,0);
    }
    
    asm volatile ("disi #5");                                                   //Disable interrupts for 5 instructions
    __builtin_write_NVM();                                                      //Initiate the unlock and erase sequence 
    while(NVMCONbits.WR);                                                       //Wait for the operation to complete
}

//Read a words from the EEPROM at the specified offset
uint16_t obee_Read(uint16_t offset){
    
    uint16_t ee_data, ee_offset;
    
    TBLPAG = __builtin_tblpage(&eedata);
    ee_offset = __builtin_tbloffset(&eedata) + offset;
    ee_data = __builtin_tblrdl(ee_offset);
    return(ee_data);
}

//Read the specified number of words sequentially, beginning at the desired offset
void obee_ReadSeq(uint16_t offset,uint16_t len,uint16_t *pBuffer){
   
   uint16_t ee_offset = offset;
   uint16_t lastWord = offset + len;
   
   while(ee_offset < lastWord){
      *pBuffer++ = obee_Read(ee_offset);
      ee_offset += WORD_LEN;
   }
}

//Write a word at the specified memory offset
void obee_Write(uint16_t wrType, uint16_t offset, uint16_t data){
    
    uint16_t ee_offset;
    
    //Flash requires the word to be erased b4 being written to
    if(wrType == EE_WRITE_ER)
       obee_Erase(EE_ERASE_ONE, offset);                                        //Erase one word                                        
    
    //Write the data word to the EEPROM
    NVMCON = wrType;                                                            //EE_WRITE_ER or EE_WRITE_NOE
    TBLPAG = __builtin_tblpage(&eedata);
    ee_offset = __builtin_tbloffset(&eedata) + offset;
    __builtin_tblwtl(ee_offset,data);
    asm volatile ("disi #5");                                                   //Disable interrupts for 5 instructions
    __builtin_write_NVM();                                                      //Initiate the unlock and erase sequence 
    while(NVMCONbits.WR);                                                       //Wait for the operation to complete
}

//Write the specified number of words to the desired memory offset
void obee_WriteSeq(uint16_t wrType, uint16_t offset, uint16_t len, uint16_t *pBuffer){
   
   uint16_t ee_offset = offset;
   uint16_t lastWord = offset + len;
   
   while(ee_offset<lastWord){
      obee_Write(wrType,ee_offset, *pBuffer);                                      //wrType = EE_WRITE_ER or EE_WRITE_NOE
      pBuffer++; ee_offset += WORD_LEN; 
   }
}