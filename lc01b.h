/* 
 * File:   lc01b.h
 */

#ifndef LC01B_H
#define	LC01B_H

#ifdef	__cplusplus
extern "C" {
#endif


#define I2C_BRG_100 157                                                         //100kHz bus: FCY = 16MHz
#define I2C_BRG_400 37                                                          //400kHz bus: FCY = 16MHz
    
//LC01B characteristics
#define LC01B_WRITE   0xA0                                                      //Control byte, write mode
#define LC01B_READ    0xA1                                                      //Control byte, read mode
#define LC01B_PAGE    8                                                         //Eight byte page size
#define LC01B_CAP     128                                                       //Memory capacity of 128 bytes
#define LC01B_MAX_ADR 0x7F                                                      //Max memory address
                                                     
//-------------------------------------------------------
// Receives: Nothing
// Returns:  Nothing
// Summary:  Sets the baud rate generator and activates
//           the I2C1 module
//-------------------------------------------------------
void init_I2C(uint8_t);

//-------------------------------------------------------
// Receives: Nothing
// Returns:  Nothing
// Summary:  Performs acknowledge polling for page writes
//-------------------------------------------------------
void ack_Poll(void);

//-------------------------------------------------------
// Receives: EEPROM memory address 
// Returns:  Nothing
// Summary:  Sends the start bit, control byte and memory 
//           address byte to the EEPROM. These are common
//           tasks for all writes and reads
//--------------------------------------------------------
void lc01b_SCM(uint8_t);                                  

//--------------------------------------------------------
// Receives: Memory address and data byte to be written
// Returns:  Status of bounds check
// Summary:  Writes a single byte to the EEPROM at the 
//           desired location
//--------------------------------------------------------
ee_Errors_t lc01b_WriteByte(uint8_t,uint8_t);

//--------------------------------------------------------
// Receives: Memory address, page length and a pointer to 
//           the data to be written
// Returns:  Nothing
// Summary:  Writes a page to the EEPROM. A page on the
//           LC01B can be to to eight bytes in length
//---------------------------------------------------------
ee_Errors_t lc01b_WritePage(uint8_t,uint8_t,uint8_t *);

//--------------------------------------------------------
// Receives: Memory address, object length and data object
// Returns:  Nothing
// Summary:  Used to write larger data types such as int, 
//           float, double etc.. to the EEPROM byte by byte
//--------------------------------------------------------
ee_Errors_t lc01b_WriteObject(uint8_t,uint8_t,void *);

//--------------------------------------------------------
// Receives: Memory address and address for data byte
// Returns:  Byte from the requested address
// Summary:  Random access read of a single byte
//--------------------------------------------------------
ee_Errors_t lc01b_ReadByte(uint8_t,uint8_t *);

//--------------------------------------------------------
// Receives: Memory address, read length and pointer to a 
//           buffer to write the output into
// Returns:  Nothing
// Summary:  Sequentially reads the requested number of
//           bytes from the EEPROM, beginning at the 
//           specified address. Output is stored in the
//           buffer provided by the client
//--------------------------------------------------------
ee_Errors_t lc01b_ReadSeq(uint8_t,uint8_t,uint8_t *);

//--------------------------------------------------------
// Receives: Memory address, data length and a pointer to
//           a data object to write the output into
// Returns:  Nothing
// Summary:  Reads the specified number of bytes from the 
//           EEPROM and writes the output to the data
//           object provided by the client
//--------------------------------------------------------
ee_Errors_t lc01b_ReadObject(uint8_t,uint8_t,void *);

#ifdef	__cplusplus
}
#endif

#endif	/* LC01B_H */

