/* Copyright 2006-2009, BeatriX
 * File coded by BeatriX
 *
 * This file is part of BeaEngine.
 *
 *    BeaEngine is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    BeaEngine is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with BeaEngine.  If not, see <http://www.gnu.org/licenses/>. */

/* ====================================================================
 *
 * ==================================================================== */
void __bea_callspec__ G15_(PDISASM pMyDisasm)
{
    GV.REGOPCODE = ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 3) & 0x7;
    if (GV.REGOPCODE == 0) {
        MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
        if (GV.MOD_!= 0x3) {
            GV.MemDecoration = Arg1multibytes;
            (*pMyDisasm).Instruction.Category = FPU_INSTRUCTION+STATE_MANAGEMENT;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "fxsave ");
            #endif
            (*pMyDisasm).Argument1.ArgSize = 512;
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+FPU_REG+MMX_REG+SSE_REG;
            (*pMyDisasm).Argument2.ArgSize = 512;
        }
        else {
            FailDecode(pMyDisasm);
        }
    }
    else if (GV.REGOPCODE == 1) {
        MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
        if (GV.MOD_!= 0x3) {
            GV.MemDecoration = Arg2multibytes;
            (*pMyDisasm).Instruction.Category = FPU_INSTRUCTION+STATE_MANAGEMENT;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "fxrstor ");
            #endif
            (*pMyDisasm).Argument2.ArgSize = 512;
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+FPU_REG+MMX_REG+SSE_REG;
            (*pMyDisasm).Argument1.ArgSize = 512;
        }
        else {
            FailDecode(pMyDisasm);
        }

    }
    else if (GV.REGOPCODE == 2) {
        MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
        if (GV.MOD_!= 0x3) {
            GV.MemDecoration = Arg2dword;
            (*pMyDisasm).Instruction.Category = SSE_INSTRUCTION+STATE_MANAGEMENT;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "ldmxcsr ");
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG1;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            FailDecode(pMyDisasm);
        }

    }
    else if (GV.REGOPCODE == 3) {
        MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
        if (GV.MOD_!= 0x3) {
            GV.MemDecoration = Arg1dword;
            (*pMyDisasm).Instruction.Category = SSE_INSTRUCTION+STATE_MANAGEMENT;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stmxcsr ");
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SPECIAL_REG+REG1;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            FailDecode(pMyDisasm);
        }

    }

    else if (GV.REGOPCODE == 4) {
        MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
        if (GV.MOD_!= 0x3) {
            GV.MemDecoration = Arg1multibytes;
            (*pMyDisasm).Instruction.Category = FPU_INSTRUCTION+STATE_MANAGEMENT;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xsave ");
            #endif
            (*pMyDisasm).Argument1.ArgSize = 512;
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+FPU_REG+MMX_REG+SSE_REG;
            (*pMyDisasm).Argument2.ArgSize = 512;
        }
        else {
            FailDecode(pMyDisasm);
        }
    }

    else if (GV.REGOPCODE == 5) {
        GV.MOD_= ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 6) & 0x3;
        if (GV.MOD_== 0x3) {
            (*pMyDisasm).Instruction.Category = SSE2_INSTRUCTION+CACHEABILITY_CONTROL;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "lfence ");
            #endif
        }
        else {
            MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
            GV.MemDecoration = Arg2multibytes;
            (*pMyDisasm).Instruction.Category = FPU_INSTRUCTION+STATE_MANAGEMENT;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xrstor ");
            #endif
            (*pMyDisasm).Argument2.ArgSize = 512;
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+FPU_REG+MMX_REG+SSE_REG;
            (*pMyDisasm).Argument1.ArgSize = 512;
        }

    }
    else if (GV.REGOPCODE == 6) {
        GV.MOD_= ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 6) & 0x3;
        if (GV.MOD_== 0x3) {
            (*pMyDisasm).Instruction.Category = SSE2_INSTRUCTION+CACHEABILITY_CONTROL;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "mfence ");
            #endif
        }
        else {
            FailDecode(pMyDisasm);
        }
    }
    else if (GV.REGOPCODE == 7) {
        GV.MOD_= ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 6) & 0x3;
        if (GV.MOD_== 0x3) {
            (*pMyDisasm).Instruction.Category = SSE2_INSTRUCTION+CACHEABILITY_CONTROL;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sfence ");
            #endif
        }
        else {
            GV.OperandSize = 8;
            MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
            GV.OperandSize = 32;
            GV.MemDecoration = Arg2byte;
            (*pMyDisasm).Instruction.Category = SSE2_INSTRUCTION+CACHEABILITY_CONTROL;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "clflush ");
            #endif
        }

    }

    else {
        FailDecode(pMyDisasm);
    }
    GV.EIP_+= GV.DECALAGE_EIP+2;
}
