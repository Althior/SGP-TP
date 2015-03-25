#include "userlib/syscall.h"
#include "userlib/libnachos.h"

LockId lock;


void f() {
  LockAcquire(lock);
  n_printf("Le processus fils à le lock !!!\n");
  LockRelease(lock);
}

int main() {
  ThreadId threadId;
  
  lock = LockCreate("lock");
  if( lock < 0 ) {
    PError("Erreur de création de verrou");
    Exit(-1);
  }

  threadId = threadCreate("thread", f);
  
  LockAcquire(lock);

  n_printf("Le processus père à le lock !!!!\n");

  LockRelease(lock);

  if( LockDestroy(lock) < 0 ) {
    PError("Erreur de destruction de verrou");
    Exit(-1);
  }

  Join(threadId);

  return 0;
}
