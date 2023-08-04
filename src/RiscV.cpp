#include "../h/RiscV.hpp"

#include "../lib/console.h"

void RiscV::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::handleSupervisorTrap() {

    uint64 scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r" (scause));

    if (scause == 0x8000000000000009UL) {
        console_handler();
    } else {
        // default
    }


}
