/*******************************************************************************
 * Interface to an MCP24LC01B EEPROM via I2C                                   
 * 
 * Resources used:
 * I2C1
 * 
 * Summary:
 * The 24LC01B:
 *   - 1 Kbit (128 byte) capacity
 *   - VCC range of 2.5-5.5V
 *   - I2C interface - 100 or 400 kHz
 *   - Page writes are 8 bytes
 *   - Page write time of ~3ms
 *  
 * Use 4K7 pullups for a 100kHz bus and 2K2 pullups on a 400kHz bus
 * Acknowledge polling is used on all writes
 * *****************************************************************************
 * 02/2023 Adam Hout           -Original source
 * ****************************************************************************/
//#include <p24F16KA102.h>
#include <xc.h>
#include "sys.h"
#include "lc01b.h"

//EEPROM error enum from sys.h
ee_Errors_t ee_Error;

//---------------------------------------------------------------
//Send the Start bit, Control byte and Memory byte to the LC01B. 
//---------------------------------------------------------------
void lc01b_SCM(uint8_t ee_addr){
   
   //Send the start bit
   I2C1CONbits.SEN = 1;                                                         //Start enable
   while(I2C1CONbits.SEN);                                                      //Wait for completion
   
   //Send the control byte                                                    
   I2C1TRN = LC01B_WRITE;                                                       //Control byte; Write mode
   while(I2C1STATbits.TRSTAT);                                                  //Wait for transmit to complete
   while(I2C1STATbits.ACKSTAT);                                                  
      
   //Send the memory address byte
   I2C1TRN = ee_addr;                                                           
   while(I2C1STATbits.TRSTAT);   
   while(I2C1STATbits.ACKSTAT);
}

//---------------------------------------------------------------p
//Write a byte to the LC01B at the desired address
//---------------------------------------------------------------
ee_Errors_t lc01b_WriteByte(uint8_t ee_addr,uint8_t dataByte){
   
    //Do a bounds check
    if(ee_addr > LC01B_MAX_ADR)
        return ERR_MEM_BOUNDS;
    else{
      //Address the EEPROM
      lc01b_SCM(ee_addr);
                                                                         
      //Send the data byte
      I2C1TRN = dataByte;                                                          
      while(I2C1STATbits.TRSTAT);                                               //Wait for transmission to complete 
      while(I2C1STATbits.ACKSTAT);
   
      //Send the stop bit
      I2C1CONbits.PEN = 1;                                                      //Stop bit
      while(I2C1CONbits.PEN);                                                   //Wait for stop to complete
      ack_Poll();
    }
    return ERR_NONE;
}

//---------------------------------------------------------------
// Page writes - Up to 8 bytes in length
//---------------------------------------------------------------
ee_Errors_t lc01b_WritePage(uint8_t ee_addr,uint8_t dataLen,uint8_t *pDataBuf){
   
  
   
  if(ee_addr > LC01B_MAX_ADR + dataLen)
      return ERR_MEM_BOUNDS;
  else{
      //Address the EEPROM
      lc01b_SCM(ee_addr);                                                       //Send START, Control byte and memory address
   
      //Write page array to the EEPROM
      for(uint8_t ctr=0;ctr<dataLen;ctr++){
         I2C1TRN = *pDataBuf++;                                                 //Next data byte
         while(I2C1STATbits.TRSTAT);
         while(I2C1STATbits.ACKSTAT);                                           //Wait for the ACK
      }   
      //Send the stop bit
      I2C1CONbits.PEN = 1;                                                      //Stop bit
      while(I2C1CONbits.PEN);                                                   //Wait for stop to complete
      //Acknowledge poll
      ack_Poll();
   }
  return ERR_NONE;
}

ee_Errors_t lc01b_WriteObject(uint8_t ee_addr,uint8_t objLen,void *pObj){
   
   uint8_t *pByte = pObj;
   
   if(ee_addr > LC01B_MAX_ADR + objLen)
       return ERR_MEM_BOUNDS;
   else{
      while(objLen--){
         lc01b_WriteByte(ee_addr++,*pByte++);
      }
   }
   return ERR_NONE;
}

ee_Errors_t lc01b_ReadByte(uint8_t ee_addr, uint8_t *pData){
   
  if(ee_addr > LC01B_MAX_ADR)
      return ERR_MEM_BOUNDS;
  else{
      //Address the EEPROM
      lc01b_SCM(ee_addr);
   
      //Repeated start
      I2C1CONbits.RSEN = 1;
      while(I2C1CONbits.RSEN);
      I2C1TRN = LC01B_READ;                                                     //Control byte; Read mode
      while(I2C1STATbits.TRSTAT);                                               //Wait for transmit to complete
      while(I2C1STATbits.ACKSTAT);
   
      //Read the EEPOM byte
      I2C1CONbits.RCEN = 1;                                                     //Receive enable 
      while(I2C1CONbits.RCEN);                                                  //Wait for the byte to shift in
      while(!I2C1STATbits.RBF);                                                 //Wait for the receive buffer flag
      *pData = I2C1RCV;                                                         //Copy from the receive buffer
      
      //Terminate the read operation
      I2C1CONbits.ACKDT = 1;                                                    //Send a NACK during acknowledge
      I2C1CONbits.ACKEN = 1;                                                    //Acknowledge enable
      while(I2C1CONbits.ACKEN);                                                 //Wait for NACK to complete
      I2C1CONbits.PEN = 1;                                                      //Stop enable
      while(I2C1CONbits.PEN);                                                   //Wait for stop to complete
   }
   return ERR_NONE;
}


ee_Errors_t lc01b_ReadSeq(uint8_t ee_addr,uint8_t readLen,uint8_t *pDataBuf){
   
   if(ee_addr > LC01B_MAX_ADR + readLen)
       return ERR_MEM_BOUNDS;
   else{
      //Address the EEPROM
      lc01b_SCM(ee_addr);
      
      //Repeated start
      I2C1CONbits.RSEN = 1;
      while(I2C1CONbits.RSEN);
      I2C1TRN = LC01B_READ;                                                     //Control byte; Read mode
      while(I2C1STATbits.TRSTAT);                                               //Wait for transmit to complete
      while(I2C1STATbits.ACKSTAT);
   
      //Read the EEPOM for the desired length
      for (uint8_t ctr=0;ctr<readLen;ctr++){
         I2C1CONbits.RCEN = 1;                                                        //Receive enable 
         while(I2C1CONbits.RCEN);                                                     //Wait for the byte to shift in
         while(!I2C1STATbits.RBF);                                                    //Wait for the receive buffer flag
         *pDataBuf++ = I2C1RCV;                                                          //Copy from the receive buffer
         if (ctr < readLen-1){                                                     //Don't ACK the last read
            I2C1CONbits.ACKDT = 0;                                                    //Send an ACK to get next byte
            I2C1CONbits.ACKEN = 1;                                                    //Acknowledge enable
            while(I2C1CONbits.ACKEN);                                                 //Wait for ACK to complete
         }
      }
      //Terminate read operation
      I2C1CONbits.ACKDT = 1;                                                       //Send a NACK during acknowledge
      I2C1CONbits.ACKEN = 1;                                                       //Acknowledge enable
      while(I2C1CONbits.ACKEN);                                                    //Wait for NACK to complete
      I2C1CONbits.PEN = 1;                                                         //Stop enable
      while(I2C1CONbits.PEN);                                                      //Wait for stop to complete  
   }
   return ERR_NONE;
}

ee_Errors_t lc01b_ReadObject(uint8_t ee_addr,uint8_t objLen, void *pObj){
   
   uint8_t *pByte = pObj;
   
   if(ee_addr > LC01B_MAX_ADR + objLen)
       return ERR_MEM_BOUNDS;
   else{
      while(objLen--){
         lc01b_ReadByte(ee_addr++,pByte++);
      }
   }
   return ERR_NONE;
}

//Acknowledge poll the EEPROM until the write cycle completes
void ack_Poll(){
   do{
      I2C1CONbits.SEN = 1;                                                      //Start enable
      while(I2C1CONbits.SEN);                                                   //Wait for completion
      I2C1TRN = LC01B_WRITE;                                                    //Control byte; Write mode
      while(I2C1STATbits.TRSTAT);                                               //Wait for transmit to complete
   }while(I2C1STATbits.ACKSTAT);                                                //Repeat until ACK'd
}  


void init_I2C(uint8_t BRG){
   I2C1CON = 0x0000;
   I2C1BRG = BRG;
   I2C1CON = 0x8000;
}