.extern _ZN5RiscV14supervisorTrapEv

.align 4
.global _ZN5RiscV10trapVectorEv
.type _ZN5RiscV10trapVectorEv, @function
_ZN5RiscV10trapVectorEv:
    j _ZN5RiscV14supervisorTrapEv
    j _ZN5RiscV14supervisorTrapEv
    unimp
    unimp
    unimp
    unimp
    unimp
    unimp
    unimp
    j _ZN5RiscV14supervisorTrapEv