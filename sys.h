/* 
 * File:   sys.h
 * Author: houta
 *
 * Created on April 5, 2023, 10:19 PM
 */

#ifndef SYS_H
#define	SYS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define FCY 16000000UL
    
//Error conditions
typedef enum{
   ERR_NONE = 0X00,                                                             //No errors
   ERR_MEM_BOUNDS = 0xE0,                                                       //Address above bounds limit
   ERR_CNTL_NACK,                                                               //NACK on control byte
   ERR_MEM_NACK,                                                                //NACK on memory address byte
   ERR_PAGE_NACK                                                                //NACK on page write
}ee_Errors_t;


#ifdef	__cplusplus
}
#endif

#endif	/* SYS_H */

