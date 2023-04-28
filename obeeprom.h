/* 
 * File:   obeeprom.h
 * Author: Adam Hout
 *
 * Library for interfacing with the PIC24F on-board EEPROM
 */

#ifndef OBEEPROM_H
#define	OBEEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EE_ERASE_ONE   0x4058
#define EE_ERASE_FOUR  0x4059
#define EE_ERASE_EIGHT 0x405A
#define EE_ERASE_BULK  0x4050
#define EE_WRITE_ER    0x4004                                                   //Write WITH erase cycle
#define EE_WRITE_NOE   0x5004                                                   //Write WITHOUT erase cycle (previously erased)
    
#define WORD_LEN  2
#define NUM_WORDS 256
    
#define OFFSET_ZERO 0
#define OFFSET_LAST 511
  
//-------------------------------------------------------
// Input:   Programming operation and address offset 
// Returns: None
// Summary: Erases 1, 4, 8 or all (bulk) words
//          beginning at the desired offset
//-------------------------------------------------------
void     obee_Erase(uint16_t, uint16_t);

//-------------------------------------------------------
// Input:   Address offset
// Returns: One data word
// Summary: Reads a single word from the EEPROM
//-------------------------------------------------------
uint16_t obee_Read(uint16_t);

//-------------------------------------------------------
// Input:   Address offset, length of data and an output 
//          buffer pointer
// Returns: None
// Summary: Copies the specified number of bytes into
//          the supplied output buffer
//-------------------------------------------------------
void     obee_ReadSeq(uint16_t,uint16_t,uint16_t *);

//-------------------------------------------------------
// Input:   Write Type, address offset and word to write 
// Returns: None
// Summary: Writes one word to the specified EEPROM 
//          address + offset.
//          wrType = EE_WRITE_ER or EE_WRITE_NOE
//-------------------------------------------------------
void     obee_Write(uint16_t, uint16_t, uint16_t);

//-------------------------------------------------------
// Input:    
// Returns:
// Summary:
void     obee_WriteSeq(uint16_t,uint16_t,uint16_t,uint16_t *);


#ifdef	__cplusplus
}
#endif

#endif	/* OBEEPROM_H */

