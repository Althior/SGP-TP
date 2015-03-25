#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int main() {
  char buff[256];

//while(1){
  TtyReceive(buff,256);
  n_printf("Chaine: %s\n", buff);
//}
    return 0;
}
