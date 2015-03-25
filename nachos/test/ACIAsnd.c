#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int main() {



    TtySend("toto\0");

    Join(0);

    return 0;
}
