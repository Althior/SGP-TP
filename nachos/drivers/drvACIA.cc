/* \file drvACIA.cc
   \brief Routines of the ACIA device driver
//
//      The ACIA is an asynchronous device (requests return 
//      immediately, and an interrupt happens later on).  
//      This is a layer on top of the ACIA.
//      Two working modes are to be implemented in assignment 2:
//      a Busy Waiting mode and an Interrupt mode. The Busy Waiting
//      mode implements a synchronous IO whereas IOs are asynchronous
//      IOs are implemented in the Interrupt mode (see the Nachos
//      roadmap for further details).
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//
*/

/* Includes */

#include "kernel/system.h"         // for the ACIA object
#include "kernel/synch.h"
#include "machine/ACIA.h"
#include "drivers/drvACIA.h"

//-------------------------------------------------------------------------
// DriverACIA::DriverACIA()
/*! Constructor.
  Initialize the ACIA driver. In the ACIA Interrupt mode, 
  initialize the reception index and semaphores and allow 
  reception and emission interrupts. 
  In the ACIA Busy Waiting mode, simply inittialize the ACIA 
  working mode and create the semaphore.
  */
//-------------------------------------------------------------------------

DriverACIA::DriverACIA()
{
  #ifndef ETUDIANTS_TP
  printf("**** Warning: contructor of the ACIA driver not implemented yet\n");
  exit(-1);
  #endif
  #ifdef ETUDIANTS_TP

  if( g_cfg->ACIA == ACIA_BUSY_WAITING ) {
    g_machine->acia->SetWorkingMode(BUSY_WAITING);

  } else if( g_cfg->ACIA == ACIA_INTERRUPT ) {

    send_sema = new Semaphore("ACIA_SEND_SEM",1);
    receive_sema = new Semaphore("ACIA_RCV_SEM",1);
    receive_mutex = new Semaphore("ACIA_RCV_MUTEX",1);
    g_machine->acia->SetWorkingMode( 0 );
    

  } else {
    // TODO

  }

  mutex = new Semaphore("ACIA_MUTEX",1);

  #endif
}

//-------------------------------------------------------------------------
// DriverACIA::TtySend(char* buff)
/*! Routine to send a message through the ACIA (Busy Waiting or Interrupt mode)
  */
//-------------------------------------------------------------------------

int DriverACIA::TtySend(char* buff)
{ 
  #ifndef ETUDIANTS_TP
  printf("**** Warning: method Tty_Send of the ACIA driver not implemented yet\n");
  exit(-1);
  #endif
  #ifdef ETUDIANTS_TP

//  mutex->P();


  if( g_cfg->ACIA  == ACIA_BUSY_WAITING ) {
    int i = -1;

    do {
      while( g_machine->acia->GetOutputStateReg() != EMPTY );
      i++;
      g_machine->acia->PutChar(buff[i]);

    } while( buff[i] != '\0'  );

    return i;
  } else {
      strncpy(send_buffer, buff, BUFFER_SIZE);
      ind_send = 0;
      send_sema->P();
      g_machine->acia->SetWorkingMode( g_machine->acia->GetWorkingMode() | SEND_INTERRUPT );
      while( g_machine->acia->GetOutputStateReg() != EMPTY );
      InterruptSend();
        
  }

  //mutex->V();

  #endif
  return 0;
}

//-------------------------------------------------------------------------
// DriverACIA::TtyReceive(char* buff,int length)
/*! Routine to reveive a message through the ACIA 
//  (Busy Waiting and Interrupt mode).
  */
//-------------------------------------------------------------------------

int DriverACIA::TtyReceive(char* buff,int lg)
{
  #ifndef ETUDIANTS_TP
   printf("**** Warning: method Tty_Receive of the ACIA driver not implemented yet\n");
  exit(-1);
  #endif
  #ifdef ETUDIANTS_TP
  if( g_cfg->ACIA  == ACIA_BUSY_WAITING ) {


    int i = 0;

    do {
        while( g_machine->acia->GetInputStateReg() != FULL );

        buff[i] = g_machine->acia->GetChar();
        i++;
    } while(buff[i-1]!='\0' && i < lg);
      return i;
  }else {

//    receive_mutex->P();


    receive_sema->P();
    ind_rec=0;
    g_machine->acia->SetWorkingMode( g_machine->acia->GetWorkingMode() | REC_INTERRUPT );

    printf("...\n");
    receive_sema->P();
    strncpy(buff,receive_buffer,lg);
    printf("c: %s\n",buff);

    receive_sema->V();
    //receive_mutex->V();
    return strlen(buff);
  }

  
  #endif
  return 0;
}


//-------------------------------------------------------------------------
// DriverACIA::InterruptSend()
/*! Emission interrupt handler.
  Used in the ACIA Interrupt mode only. 
  Detects when it's the end of the message (if so, releases the send_sema semaphore), else sends the next character according to index ind_send.
  */
//-------------------------------------------------------------------------

void DriverACIA::InterruptSend()
{
  #ifdef ETUDIANTS_TP
    char c = send_buffer[ind_send];
    g_machine->acia->PutChar(c);

    if( c == '\0' ) {
      g_machine->acia->SetWorkingMode( g_machine->acia->GetWorkingMode() & REC_INTERRUPT );
      send_sema->V();
    } else {
      ind_send++;
    }
  #endif
  #ifndef ETUDIANTS_TP
    printf("**** Warning: send interrupt handler not implemented yet\n");
    exit(-1);
  #endif
}

//-------------------------------------------------------------------------
// DriverACIA::Interrupt_receive()
/*! Reception interrupt handler.
  Used in the ACIA Interrupt mode only. Reveices a character through the ACIA. 
  Releases the receive_sema semaphore and disables reception 
  interrupts when the last character of the message is received 
  (character '\0').
  */
//-------------------------------------------------------------------------

void DriverACIA::InterruptReceive()
{
  #ifndef ETUDIANTS_TP
    printf("**** Warning: receive interrupt handler not implemented yet\n");
    exit(-1);
  #endif
  #ifdef ETUDIANTS_TP
    char c = g_machine->acia->GetChar();
    receive_buffer[ind_rec] = c;

    if( c == '\0' ) {
printf("c: %s\n",receive_buffer);
      g_machine->acia->SetWorkingMode( g_machine->acia->GetWorkingMode() & SEND_INTERRUPT );
      receive_sema->V();


    } else {
      ind_rec++;

    }
  #endif
}
