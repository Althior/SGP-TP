//-----------------------------------------------------------------
/*! \file mem.cc
//  \brief Routines for the physical page management
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//-----------------------------------------------------------------

#include <unistd.h>
#include "vm/physMem.h"

//-----------------------------------------------------------------
// PhysicalMemManager::PhysicalMemManager
//
/*! Constructor. It simply clears all the page flags and inserts them in the
// free_page_list to indicate that the physical pages are free
*/
//-----------------------------------------------------------------
PhysicalMemManager::PhysicalMemManager() {

  long i;

  tpr = new struct tpr_c[g_cfg->NumPhysPages];

  for (i=0;i<g_cfg->NumPhysPages;i++) {
    tpr[i].free=true;
    tpr[i].locked=false;
    tpr[i].owner=NULL;
    free_page_list.Append((void*)i);
  }
  i_clock=-1;
}

PhysicalMemManager::~PhysicalMemManager() {

  delete[] tpr;
}

//-----------------------------------------------------------------
// PhysicalMemManager::RemovePhysicalToVitualMapping
//
/*! This method releases an unused physical page by clearing the
//  corresponding bit in the page_flags bitmap structure, and adding
//  it in the free_page_list.
//
//  \param num_page is the number of the real page to free
*/
//-----------------------------------------------------------------
void PhysicalMemManager::RemovePhysicalToVirtualMapping(long num_page) {
  
  // Check that the page is not already free 
  ASSERT(!tpr[num_page].free);

  // Update the physical page table entry
  tpr[num_page].free=true;
  tpr[num_page].locked=false;
  if (tpr[num_page].owner->translationTable!=NULL) 
    tpr[num_page].owner->translationTable->clearBitValid(tpr[num_page].virtualPage);

  // Insert the page in the free list
  free_page_list.Prepend((void*)num_page);
}

//-----------------------------------------------------------------
// PhysicalMemManager::UnlockPage
//
/*! This method unlocks the page numPage, after
//  checking the page is in the locked state. Used
//  by the page fault manager to unlock at the
//  end of a page fault (the page cannot be evicted until
//  the page fault handler terminates).
//
//  \param num_page is the number of the real page to unlock
*/
//-----------------------------------------------------------------
void PhysicalMemManager::UnlockPage(long num_page) {
  ASSERT(num_page<g_cfg->NumPhysPages);
  ASSERT(tpr[num_page].locked==true);
  ASSERT(tpr[num_page].free==false);
  tpr[num_page].locked = false;
}

//-----------------------------------------------------------------
// PhysicalMemManager::ChangeOwner
//
/*! Change the owner of a page
//
//  \param owner is a pointer on new owner (Thread *)
//  \param numPage is the concerned page
*/
//-----------------------------------------------------------------
void PhysicalMemManager::ChangeOwner(long numPage, Thread* owner) {
  // Update statistics
  g_current_thread->GetProcessOwner()->stat->incrMemoryAccess();
  // Change the page owner
  tpr[numPage].owner = owner->GetProcessOwner()->addrspace;
}

//-----------------------------------------------------------------
// PhysicalMemManager::AddPhysicalToVirtualMapping 
//
/*! This method returns a new physical page number. If there is no
//  page available, it evicts one page (page replacement algorithm).
//
//  NB: this method locks the newly allocated physical page such that
//      it is not stolen during the page fault resolution. Don't forget
//      to unlock it
//
//  \param owner address space (for backlink)
//  \param virtualPage is the number of virtualPage to link with physical page
//  \return A new physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::AddPhysicalToVirtualMapping(AddrSpace* owner,int virtualPage) 
{
  int page;

  // First try to find a free page
  page=FindFreePage();  

  // If there is no free page, call the replacement algorithm
  if (page==-1) {page=EvictPage();}  // no free page : clock

  // Update the physical page table
  tpr[page].virtualPage=virtualPage;
  tpr[page].owner=owner;
  tpr[page].locked=true; // Lock the page until it is filled in, it could then get evicted
  DEBUG('v',(char *)"AddPhysicalToVirtualMapping, virtual : %i, real : %i\n",virtualPage,page);  
  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::FindFreePage
//
/*! This method returns a new physical page number, if it finds one
//  free. If not, return -1. Does not run the clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::FindFreePage() {
  int64_t page;

  // Check that the free list is not empty
  if (free_page_list.IsEmpty())
    return -1;

  // Update statistics
  g_current_thread->GetProcessOwner()->stat->incrMemoryAccess();
  
  // Get a page from the free list
  page = (int64_t)free_page_list.Remove();
  
  // Check that the page is really free
  ASSERT(tpr[page].free);
  
  // Update the physical page table
  tpr[page].free = false;

  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::EvictPage
//
/*! This method implements page replacement, using the well-known
//  clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::EvictPage() {
#ifndef ETUDIANTS_TP
	printf("**** Warning: page replacement algorithm is not implemented yet\n");
	exit(-1);
	return (0);
#endif
#ifdef ETUDIANTS_TP
	// clock algo.
	int i = 0;
	do {
			i_clock = (i_clock + 1) % g_cfg->NumPhysPages;
			if( ! tpr[i_clock].locked ) {
				tpr[i_clock].owner->translationTable->clearBitU(tpr[i_clock].virtualPage);
			} else {
				i++;
			}
			if( i >= g_cfg->NumPhysPages ) {
				IntStatus oldStatus = g_machine->interrupt->GetStatus();
  				g_machine->interrupt-> SetStatus(INTERRUPTS_OFF);
  				g_current_thread->SetIClock(i_clock);
				g_current_thread->Sleep();
				i_clock = g_current_thread->GetIClock();
				g_machine->interrupt-> SetStatus(oldStatus);
			}
	} while (
		tpr[i_clock].locked
		&& tpr[i_clock].owner->translationTable->getBitU(tpr[i_clock].virtualPage) 
	);
	
	tpr[i_clock].locked = true;
	tpr[i_clock].owner->translationTable->clearBitValid(tpr[i_clock].virtualPage);
	
	int res = i_clock;

	// copy page in swap.
	TranslationTable* tt = tpr[res].owner->translationTable;
	int vpn = tpr[i_clock].virtualPage;
	
	while( tt->getBitIo(vpn) ) {
		IntStatus oldStatus = g_machine->interrupt->GetStatus();
		g_machine->interrupt-> SetStatus(INTERRUPTS_OFF);
		g_current_thread->Sleep();
		g_machine->interrupt-> SetStatus(oldStatus);			
	}
	tt->setBitIo(vpn);
	
	if ( tt->getBitSwap(vpn) ) {
		if( tt->getBitM(vpn) ) {
			g_swap_manager->PutPageSwap(tt->getAddrDisk(vpn), (char*) (g_machine->mainMemory+res*g_cfg->PageSize));
		}
	} else {	
		int swapAddr = g_swap_manager->PutPageSwap(-1, (char*) (g_machine->mainMemory+res*g_cfg->PageSize));
		tt->setAddrDisk(vpn, swapAddr);
		tt->setBitSwap(vpn);
	}
	
	tt->clearBitIo(vpn);
	return res;
	
#endif
}

//-----------------------------------------------------------------
// PhysicalMemManager::Print
//
/*! print the current status of the table of physical pages
//
//  \param rpage number of real page
*/
//-----------------------------------------------------------------

void PhysicalMemManager::Print(void) {
  int i;

  printf("Contents of TPR (%d pages)\n",g_cfg->NumPhysPages);
  for (i=0;i<g_cfg->NumPhysPages;i++) {
    printf("Page %d free=%d locked=%d virtpage=%d owner=%lx U=%d M=%d\n",
	   i,
	   tpr[i].free,
	   tpr[i].locked,
	   tpr[i].virtualPage,
	   (long int)tpr[i].owner,
	   (tpr[i].owner!=NULL) ? tpr[i].owner->translationTable->getBitU(tpr[i].virtualPage) : 0,
	   (tpr[i].owner!=NULL) ? tpr[i].owner->translationTable->getBitM(tpr[i].virtualPage) : 0);
  }
}
