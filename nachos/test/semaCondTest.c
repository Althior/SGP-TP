#include "userlib/syscall.h"
#include "userlib/libnachos.h"

#define N_THREADS 10

SemId sema;
CondId cond;
int i;

void f() {
  int j = i;
  CondSignal(cond);
  P(sema);
  n_printf("Je suis le fils %d !\n",j);
  V(sema);
}



int main() {

  ThreadId threadIds[N_THREADS];

  sema = SemCreate("sema",1);
  if( sema < 0 ) {
    PError("Erreur de création de sémaphore");
  }

  cond = CondCreate("cond");
  if( cond < 0 ) {
    PError("Erreur de création de variable condition");
  }

n_printf("dsqfjhlkdsqfjkmlsqdfkmj\n");
  for( i=0; i < N_THREADS; i++ ) {
    // création des threads fils.
    threadIds[i] = threadCreate("thread", f);
    CondWait(cond);
  }
  
n_printf("dsqfjhlksqkjfdhmlkdsqjmokf\n");
  for( i=0; i < N_THREADS; i++ ) {
    // attente de la fin des threads fils.
  	Join(threadIds[i]);	
  }



  if( SemDestroy(sema) < 0 ) {
    PError("Erreur de destruction de sémaphore");
  }

  if( CondDestroy(cond) < 0 ) {
    PError("Erreur de destruction de variable condition");
  }

	n_printf("Fin proc pere\n");

  return 0;
}
