# shibumi | 渋味
### **_"Shibui (adjective), shibumi (noun) or shibusa (noun) are Japanese words that refer to a particular aesthetic or beauty that is simple, subtle and not showy."_**
[![CodeFactor](https://www.codefactor.io/repository/github/cocosimone/shibumi/badge)](https://www.codefactor.io/repository/github/cocosimone/shibumi)

Experimental Nintendo 64 emulator in C11.

95% of Krom's tests are currently passing. Details as follows:
- [x] ADD/ADDI: 100%
- [X] ADDU/ADDIU: 100%
- [x] AND: 100%
- [x] NOR: 100%
- [x] OR: 100%
- [x] XOR: 100%
- [x] DADD/DADDI: 100%
- [x] DADDU/DADDIU: 100%
- [x] DDIV/DDIVU: 100% if you ignore that DDIV crashes on SIGFPE, and won't be fixed because it's a dumb edge case no game uses :upside_down_face:
- [x] DIV/DIVU: 100%
- [x] DMULT: 100%
- [x] DMULTU: 100%
- [x] DSSL/DSSL32/DSSLV/DSRA/DSRA32/DSRAV/DSRL/DSRL32/DSRLV: 100%
- [x] DSUB/DSUBU: 100%
- [x] LB/LBU: 100%
- [x] LH/LHU: 100%
- [x] LW/LWL/LWR: 100%
- [x] LD/LDL/LDR: 100%
- [x] MULT/MULTU: 100%
- [x] SB: 100%
- [x] SH: 100%
- [x] SW/SWL/SWR: 100%
- [x] SD/SDL/SDR: 100%
- [x] SLL/SLLV/SRA/SRAV/SRL/SRLV: 100%
- [x] SUB/SUBU: 100%
- [ ] CPUTIMINGNTSC: 0%, because I only have a very basic framebuffer implemented and no timing. I'll integrate [paraLLEl-rdp](https://github.com/Themaister/parallel-rdp-standalone) eventually and make an effort to pass it. Not my highest priority.
- [ ] LL_LLD_SC_SCD: 0%, I have no idea what this is :sweat_smile: