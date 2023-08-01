#ifndef PROJECT_BASE_H_RISCV_H
#define PROJECT_BASE_H_RISCV_H


#include "../lib/hw.h"


class RiscV {
public:

    static void supervisorTrap();



private:
    static void handleSupervisorTrap();

};


#endif // PROJECT_BASE_H_RISCV_H