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
 *      90h
 * ==================================================================== */
void __bea_callspec__ nop_(PDISASM pMyDisasm)
{
    if (GV.PrefRepe == 1) {
        (*pMyDisasm).Instruction.Category = SSE2_INSTRUCTION+CACHEABILITY_CONTROL;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pause ");
        #endif
        GV.EIP_++;
    }
    else {
        if (GV.REX.B_ == 1) {
            if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
                (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
            }
            (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
            #endif
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((char*) (*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
            (*pMyDisasm).Argument1.ArgSize = 64;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((char*) (*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0+8];
            (*pMyDisasm).Argument2.ArgSize = 64;
            GV.EIP_++;
        }
        else {
            (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+MISCELLANEOUS_INSTRUCTION;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "nop ");
            #endif
            GV.EIP_++;
        }
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ nop_Ev(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+MISCELLANEOUS_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "nop ");
    #endif
    if (GV.OperandSize == 64) {
        GV.MemDecoration = Arg2qword;
    }
    else if (GV.OperandSize == 32) {
        GV.MemDecoration = Arg2dword;
    }
    else {
        GV.MemDecoration = Arg2word;
    }
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.EIP_ += GV.DECALAGE_EIP+2;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ hint_nop(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+MISCELLANEOUS_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "hint_nop ");
    #endif
    if (GV.OperandSize == 64) {
        GV.MemDecoration = Arg2qword;
    }
    else if (GV.OperandSize == 32) {
        GV.MemDecoration = Arg2dword;
    }
    else {
        GV.MemDecoration = Arg2word;
    }
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.EIP_ += GV.DECALAGE_EIP+2;
}

/* =======================================
 *      08h
 * ======================================= */
void __bea_callspec__ or_EbGb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "or ");
    #endif
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,74);
}

/* =======================================
 *      09h
 * ======================================= */
void __bea_callspec__ or_EvGv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "or ");
    #endif
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,74);
}

/* =======================================
 *      0ah
 * ======================================= */
void __bea_callspec__ or_GbEb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "or ");
    #endif
    GbEb(pMyDisasm);
    FillFlags(pMyDisasm,74);
}

/* =======================================
 *      0bh
 * ======================================= */
void __bea_callspec__ or_GvEv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "or ");
    #endif
    GvEv(pMyDisasm);
    FillFlags(pMyDisasm,74);
}

/* =======================================
 *      0ch
 * ======================================= */
void __bea_callspec__ or_ALIb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "or ");
    #endif
    ALIb(pMyDisasm);
    FillFlags(pMyDisasm,74);
}

/* =======================================
 *      0dh
 * ======================================= */
void __bea_callspec__ or_eAX_Iv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "or ");
    #endif
    eAX_Iv(pMyDisasm);
    FillFlags(pMyDisasm,74);
}


/* =======================================
 *      06eh
 * ======================================= */
void __bea_callspec__ outsb_(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.RepnePrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepnePrefix = InUsePrefix;
    }
    if ((*pMyDisasm).Prefix.RepPrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+InOutINSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "outsb ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG2;
    (*pMyDisasm).Argument1.ArgSize = 16;
    (*pMyDisasm).Argument1.AccessMode = READ;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG6;
    (*pMyDisasm).Argument2.ArgSize = 8;
    GV.EIP_++;
    FillFlags(pMyDisasm, 76);
}

/* =======================================
 *      06fh
 * ======================================= */
void __bea_callspec__ outsw_(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.RepnePrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepnePrefix = InUsePrefix;
    }
    if ((*pMyDisasm).Prefix.RepPrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+InOutINSTRUCTION;
    if (GV.OperandSize >= 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "outsd ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG2;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.AccessMode = READ;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG6;
        (*pMyDisasm).Argument2.ArgSize = 32;
        GV.EIP_++;
        FillFlags(pMyDisasm, 76);
    }

    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "outsw ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG2;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.AccessMode = READ;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG6;
        (*pMyDisasm).Argument2.ArgSize = 16;
        GV.EIP_++;
        FillFlags(pMyDisasm, 76);
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ out_IbAL(PDISASM pMyDisasm)
{
    long MyNumber;

    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+InOutINSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "out ");
    #endif
    if (!Security(2, pMyDisasm)) return;
    GV.ImmediatSize = 8;
    MyNumber = *((UInt8*)(UIntPtr) (GV.EIP_+1));
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument1.ArgMnemonic,"%.2X",(Int64) MyNumber);
    #endif
    (*pMyDisasm).Argument1.ArgType = CONSTANT_TYPE+ABSOLUTE_;
    (*pMyDisasm).Argument1.ArgSize = 8;
    (*pMyDisasm).Argument1.AccessMode = READ;
    (*pMyDisasm).Instruction.Immediat = MyNumber;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers8Bits[0]);
    #endif
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
    (*pMyDisasm).Argument2.ArgSize = 8;
    GV.EIP_ += 2;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ out_Ib_eAX(PDISASM pMyDisasm)
{
    long MyNumber;

    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+InOutINSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "out ");
    #endif
    if (!Security(2, pMyDisasm)) return;
    GV.ImmediatSize = 8;
    MyNumber = *((UInt8*)(UIntPtr) (GV.EIP_+1));
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument1.ArgMnemonic,"%.2X",(Int64) MyNumber);
    #endif
    (*pMyDisasm).Argument1.ArgSize = 8;
    (*pMyDisasm).Argument1.AccessMode = READ;
    (*pMyDisasm).Instruction.Immediat = MyNumber;
    if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[0]);
        #endif
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[0]);
        #endif
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
    GV.EIP_ += 2;
}


/* =======================================
 *
 * ======================================= */
void __bea_callspec__ out_DXAL(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+InOutINSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "out ");
    #endif
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[2]);
    #endif
    (*pMyDisasm).Argument1.ArgSize = 16;
    (*pMyDisasm).Argument1.AccessMode = READ;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers8Bits[0]);
    #endif
    (*pMyDisasm).Argument2.ArgSize = 8;
    GV.EIP_ ++;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ out_DXeAX(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+InOutINSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "out ");
    #endif
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[2]);
    #endif
    (*pMyDisasm).Argument1.ArgSize = 16;
    (*pMyDisasm).Argument1.AccessMode = READ;
    if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[0]);
        #endif
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[0]);
        #endif
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
    GV.EIP_ ++;
}

/* =======================================
 *      8fh
 * ======================================= */
void __bea_callspec__ pop_Ev(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    GV.REGOPCODE = ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 3) & 0x7;
    if (GV.REGOPCODE == 0) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
        #endif
        if (GV.Architecture == 64) {
            GV.OperandSize = 64;
            GV.MemDecoration = Arg1qword;
            MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
            GV.EIP_ += GV.DECALAGE_EIP+2;
            (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
            (*pMyDisasm).Argument2.ArgSize = 64;
            (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
            (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        }
        else if (GV.OperandSize == 32) {
            GV.MemDecoration = Arg1dword;
            MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
            GV.EIP_ += GV.DECALAGE_EIP+2;
            (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
            (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        }
        else {
            GV.MemDecoration = Arg1word;
            MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
            GV.EIP_ += GV.DECALAGE_EIP+2;
            (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
            (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        }
    }
    else {
        FailDecode(pMyDisasm);
    }
}

/* =======================================
 *      58h
 * ======================================= */
void __bea_callspec__ pop_eax(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      59h
 * ======================================= */
void __bea_callspec__ pop_ecx(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[1+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[1+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[1+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[1+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      5ah
 * ======================================= */
void __bea_callspec__ pop_edx(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[2+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2];

        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[2+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[2+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[2+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      5bh
 * ======================================= */
void __bea_callspec__ pop_ebx(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[3+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[3+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[3+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[3+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      5ch
 * ======================================= */
void __bea_callspec__ pop_esp(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[4+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[4+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[4+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[4+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      5dh
 * ======================================= */
void __bea_callspec__ pop_ebp(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[5+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[5+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[5+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[5+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      5eh
 * ======================================= */
void __bea_callspec__ pop_esi(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[6+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[6+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[6+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[6+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      5fh
 * ======================================= */
void __bea_callspec__ pop_edi(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[7+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[7+0+8]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7+8];
        }
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[7+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7];
            (*pMyDisasm).Argument1.ArgSize = 32;
            (*pMyDisasm).Argument2.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[7+0]);
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7];
            (*pMyDisasm).Argument1.ArgSize = 16;
            (*pMyDisasm).Argument2.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ pop_es(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[0]);
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[0];
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ pop_ss(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[2]);
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[2];
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ pop_ds(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[3]);
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[3];
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ pop_fs(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[4]);
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[4];
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ pop_gs(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pop ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[5]);
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[5];
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *      9dh
 * ======================================= */
void __bea_callspec__ popfd_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+FLAG_CONTROL_INSTRUCTION;

    if (GV.Architecture == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "popfq ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
        FillFlags(pMyDisasm, 78);
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "popfd ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
        FillFlags(pMyDisasm, 78);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "popf ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
        FillFlags(pMyDisasm, 78);
    }
}

/* =======================================
 *      9dh
 * ======================================= */
void __bea_callspec__ popad_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;

    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "popad ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG1+REG2+REG3+REG4+REG5+REG6+REG7;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "popa ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG1+REG2+REG3+REG4+REG5+REG6+REG7;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
    }
}


/* =======================================
 *
 * ======================================= */
void __bea_callspec__ push_es(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[0]);
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[0];
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ push_ss(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[2]);
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[2];
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ push_ds(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[3]);
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[3];
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ push_fs(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[4]);
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[4];
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ push_gs(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[5]);
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[5];
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ push_cs(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy((char*) &(*pMyDisasm).Argument1.ArgMnemonic, RegistersSEG[1]);
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SEGMENT_REG+REGS[1];
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
        GV.EIP_++;
    }
}



/* =======================================
 *      50h
 * ======================================= */
void __bea_callspec__ push_eax(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[0];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      51h
 * ======================================= */
void __bea_callspec__ push_ecx(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[1+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[1+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[1+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[1+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[1];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      52h
 * ======================================= */
void __bea_callspec__ push_edx(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[2+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2];

        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[2+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[2+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[2+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[2];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      53h
 * ======================================= */
void __bea_callspec__ push_ebx(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[3+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[3+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[3+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[3+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[3];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      54h
 * ======================================= */
void __bea_callspec__ push_esp(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[4+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[4+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[4+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[4+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[4];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      55h
 * ======================================= */
void __bea_callspec__ push_ebp(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[5+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[5+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[5+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[5+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[5];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      56h
 * ======================================= */
void __bea_callspec__ push_esi(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[6+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[6+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[6+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[6+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[6];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}

/* =======================================
 *      57h
 * ======================================= */
void __bea_callspec__ push_edi(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (GV.REX.B_ == 0) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[7+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7];
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[7+0+8]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7+8];
        }
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        if (GV.OperandSize == 32) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[7+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7];
            (*pMyDisasm).Argument2.ArgSize = 32;
            (*pMyDisasm).Argument1.ArgSize = 32;
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy((char*) &(*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[7+0]);
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REGS[7];
            (*pMyDisasm).Argument2.ArgSize = 16;
            (*pMyDisasm).Argument1.ArgSize = 16;
        }
    }
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
}


/* =======================================
 *      68h
 * ======================================= */
void __bea_callspec__ push_Iv(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;

    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (GV.Architecture == 64) {
        if (!Security(5, pMyDisasm)) return;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument2.ArgMnemonic,"%.8X",(Int64)*((UInt32*)(UIntPtr) (GV.EIP_+1)));
        #endif
        GV.ImmediatSize = 32;
        (*pMyDisasm).Instruction.Immediat = *((UInt32*)(UIntPtr) (GV.EIP_+1));
        GV.EIP_ += 5;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Argument2.ArgType = CONSTANT_TYPE+ABSOLUTE_;
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
    }
    else if (GV.OperandSize == 32) {
        if (!Security(5, pMyDisasm)) return;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument2.ArgMnemonic,"%.8X",(Int64)*((UInt32*)(UIntPtr) (GV.EIP_+1)));
        #endif
        GV.ImmediatSize = 32;
        (*pMyDisasm).Instruction.Immediat = *((UInt32*)(UIntPtr) (GV.EIP_+1));
        GV.EIP_ += 5;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Argument2.ArgType = CONSTANT_TYPE+ABSOLUTE_;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
    }
    else {
        if (!Security(3, pMyDisasm)) return;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument2.ArgMnemonic,"%.4X",(Int64)*((UInt16*)(UIntPtr) (GV.EIP_+1)));
        #endif
        GV.ImmediatSize = 16;
        (*pMyDisasm).Instruction.Immediat = *((UInt16*)(UIntPtr) (GV.EIP_+1));
        GV.EIP_ += 3;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Argument2.ArgType = CONSTANT_TYPE+ABSOLUTE_;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;
    }
}

/* =======================================
 *      6ah
 * ======================================= */
void __bea_callspec__ push_Ib(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "push ");
    #endif
    if (!Security(1, pMyDisasm)) return;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument2.ArgMnemonic,"%.8X",(Int64)*((Int8*)(IntPtr) (GV.EIP_+1)));
    #endif
    GV.ImmediatSize = 8;
    (*pMyDisasm).Instruction.Immediat = *((UInt8*)(UIntPtr) (GV.EIP_+1));
    GV.EIP_ += 2;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.ArgSize = 32;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
    (*pMyDisasm).Argument2.ArgType = CONSTANT_TYPE+ABSOLUTE_;
    (*pMyDisasm).Argument2.ArgSize = 8;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = REGISTER_TYPE+GENERAL_REG+REG4;

}


/* =======================================
 *      9ch
 * ======================================= */
void __bea_callspec__ pushfd_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+FLAG_CONTROL_INSTRUCTION;

    if (GV.Architecture == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pushfq ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
        FillFlags(pMyDisasm, 78);
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pushfd ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
        FillFlags(pMyDisasm, 78);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pushf ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
        FillFlags(pMyDisasm, 78);
    }
}


/* =======================================
 *      60h
 * ======================================= */
void __bea_callspec__ pushad_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;

    if (GV.Architecture == 64) {
        FailDecode(pMyDisasm);
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pushad ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG1+REG2+REG3+REG4+REG5+REG6+REG7;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "pusha ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG1+REG2+REG3+REG4+REG5+REG6+REG7;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG4;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
    }
}

/* =======================================
 *      0c2h
 * ======================================= */
void __bea_callspec__ retn_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+CONTROL_TRANSFER;
    (*pMyDisasm).Instruction.BranchType = RetType;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "retn ");
    #endif
    if (!Security(3, pMyDisasm)) return;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument1.ArgMnemonic,"%.4X",(Int64)*((UInt16*)(UIntPtr) (GV.EIP_+1)));
    #endif
    GV.ImmediatSize = 16;
    (*pMyDisasm).Instruction.Immediat = *((UInt16*)(UIntPtr) (GV.EIP_+1));
    GV.EIP_+=3;
    (*pMyDisasm).Argument1.ArgType = CONSTANT_TYPE+ABSOLUTE_;
    (*pMyDisasm).Argument1.ArgSize = 16;
    (*pMyDisasm).Argument1.AccessMode = READ;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
}

/* =======================================
 *      0c3h
 * ======================================= */
void __bea_callspec__ ret_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+CONTROL_TRANSFER;
    (*pMyDisasm).Instruction.BranchType = RetType;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "ret ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.ArgSize = 32;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ retf_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+CONTROL_TRANSFER;
    (*pMyDisasm).Instruction.BranchType = RetType;
    if (GV.SYNTAX_ == ATSyntax) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "lret ");
        #endif
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "retf ");
        #endif
    }
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "retf ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.ArgSize = 32;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG4;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ retf_Iw(PDISASM pMyDisasm)
{
    int i = 0;
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+CONTROL_TRANSFER;
    (*pMyDisasm).Instruction.BranchType = RetType;
    if (GV.SYNTAX_ == ATSyntax) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "lret ");
        #endif
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic,"\x24");
        #endif
        i++;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "retf ");
        #endif
    }
    if (!Security(3, pMyDisasm)) return;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) &(*pMyDisasm).Argument1.ArgMnemonic+i,"%.4X",(Int64)*((UInt16*)(UIntPtr) (GV.EIP_+1)));
    #endif
    GV.ImmediatSize = 16;
    (*pMyDisasm).Instruction.Immediat = *((UInt16*)(UIntPtr) (GV.EIP_+1));
    GV.EIP_+=3;
    (*pMyDisasm).Argument1.ArgType = CONSTANT_TYPE+ABSOLUTE_;
    (*pMyDisasm).Argument1.ArgSize = 16;
    (*pMyDisasm).Argument1.AccessMode = READ;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG4;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ rdtsc_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "rdtsc ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG2;
    (*pMyDisasm).Argument1.ArgSize = 32;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ rdmsr_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "rdmsr ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG2;
    (*pMyDisasm).Argument1.ArgSize = 32;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG1;
    (*pMyDisasm).Argument2.ArgSize = 32;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ rdpmc_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "rdpmc ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG2;
    (*pMyDisasm).Argument1.ArgSize = 32;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG1;
    (*pMyDisasm).Argument2.ArgSize = 32;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ rsm_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "rsm ");
    #endif
    GV.EIP_++;
    FillFlags(pMyDisasm, 89);
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ sysenter_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sysenter ");
    #endif
    GV.EIP_++;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ sysexit_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sysexit ");
    #endif
    GV.EIP_++;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ sahf_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+FLAG_CONTROL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sahf ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
    (*pMyDisasm).Argument1.ArgSize = 32;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgSize = 8;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ salc_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = UNDOCUMENTED_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "salc ");
    #endif
    GV.EIP_++;
}


/* =======================================
 *      0ach
 * ======================================= */
void __bea_callspec__ scasb_(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.RepnePrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepnePrefix = InUsePrefix;
    }
    if ((*pMyDisasm).Prefix.RepPrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+STRING_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "scasb ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument1.ArgSize = 8;
    (*pMyDisasm).Argument1.AccessMode = READ;
    (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument2.Memory.BaseRegister = REG7;
    (*pMyDisasm).Argument2.ArgSize = 8;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
    GV.EIP_++;
    FillFlags(pMyDisasm, 94);
}

/* =======================================
 *      0adh
 * ======================================= */
void __bea_callspec__ scas_(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.RepnePrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepnePrefix = InUsePrefix;
    }
    if ((*pMyDisasm).Prefix.RepPrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+STRING_INSTRUCTION;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "scasq ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument1.AccessMode = READ;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG7;
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
        GV.EIP_++;
        FillFlags(pMyDisasm, 94);
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "scasd ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument1.AccessMode = READ;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG7;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
        GV.EIP_++;
        FillFlags(pMyDisasm, 94);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "scasw ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument1.AccessMode = READ;
        (*pMyDisasm).Argument2.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument2.Memory.BaseRegister = REG7;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
        GV.EIP_++;
        FillFlags(pMyDisasm, 94);
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ stc_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+FLAG_CONTROL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stc ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
    (*pMyDisasm).Argument1.ArgSize = 1;
    FillFlags(pMyDisasm, 98);
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ sti_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+FLAG_CONTROL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sti ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
    (*pMyDisasm).Argument1.ArgSize = 1;
    FillFlags(pMyDisasm, 100);
}


/* =======================================
 *      0aah
 * ======================================= */
void __bea_callspec__ stos_(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.RepnePrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepnePrefix = InUsePrefix;
    }
    if ((*pMyDisasm).Prefix.RepPrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+STRING_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stosb ");
    #endif
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgSize = 8;
    (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
    (*pMyDisasm).Argument1.Memory.BaseRegister = REG7;
    (*pMyDisasm).Argument1.ArgSize = 8;
    (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
    GV.EIP_++;
    FillFlags(pMyDisasm, 101);
}

/* =======================================
 *      0abh
 * ======================================= */
void __bea_callspec__ stosw_(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.RepnePrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepnePrefix = InUsePrefix;
    }
    if ((*pMyDisasm).Prefix.RepPrefix == SuperfluousPrefix) {
        (*pMyDisasm).Prefix.RepPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+STRING_INSTRUCTION;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stosq ");
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 64;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG7;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
        GV.EIP_++;
        FillFlags(pMyDisasm, 101);
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stosd ");
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 32;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG7;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
        GV.EIP_++;
        FillFlags(pMyDisasm, 101);
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stosw ");
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 16;
        (*pMyDisasm).Argument1.ArgType = MEMORY_TYPE;
        (*pMyDisasm).Argument1.Memory.BaseRegister = REG7;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Instruction.ImplicitModifiedRegs = GENERAL_REG+REG7;
        GV.EIP_++;
        FillFlags(pMyDisasm, 101);
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ syscall_(PDISASM pMyDisasm)
{
    if (GV.Architecture == 64) {
        (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION+CONTROL_TRANSFER;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "syscall ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG1+REG11;
        (*pMyDisasm).Argument1.ArgSize = 64;
    }
    else {
        FailDecode(pMyDisasm);
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ sysret_(PDISASM pMyDisasm)
{
    if (GV.Architecture == 64) {
        (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sysret ");
        #endif
        GV.EIP_++;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG1+REG11;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else {
        FailDecode(pMyDisasm);
    }
}



/* =======================================
 *      18h
 * ======================================= */
void __bea_callspec__ sbb_EbGb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sbb ");
    #endif
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,93);
}

/* =======================================
 *      19h
 * ======================================= */
void __bea_callspec__ sbb_EvGv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sbb ");
    #endif
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,93);
}

/* =======================================
 *      1ah
 * ======================================= */
void __bea_callspec__ sbb_GbEb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sbb ");
    #endif
    GbEb(pMyDisasm);
    FillFlags(pMyDisasm,93);
}

/* =======================================
 *      1bh
 * ======================================= */
void __bea_callspec__ sbb_GvEv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sbb ");
    #endif
    GvEv(pMyDisasm);
    FillFlags(pMyDisasm,93);
}

/* =======================================
 *      1ch
 * ======================================= */
void __bea_callspec__ sbb_ALIb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sbb ");
    #endif
    ALIb(pMyDisasm);
    FillFlags(pMyDisasm,93);
}

/* =======================================
 *      1dh
 * ======================================= */
void __bea_callspec__ sbb_eAX_Iv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sbb ");
    #endif
    eAX_Iv(pMyDisasm);
    FillFlags(pMyDisasm,93);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ seto_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "seto ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f91h
 * ======================================= */
void __bea_callspec__ setno_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setno ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setb_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setb ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setnb_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setnb ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ sete_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sete ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setne_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setne ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setbe_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setbe ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setnbe_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setnbe ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ sets_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sets ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setns_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setns ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setp_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setp ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setnp_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setnp ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setge_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setnl ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setnge_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setl ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setle_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setle ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}

/* =======================================
 *      0f90h
 * ======================================= */
void __bea_callspec__ setnle_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "setnle ");
    #endif
    GV.MemDecoration = Arg2byte;
    GV.OperandSize = 8;
    MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
    GV.OperandSize = 32;
    GV.EIP_+= GV.DECALAGE_EIP+2;
    FillFlags(pMyDisasm,95);
}


/* =======================================
 *      0fa4h
 * ======================================= */
void __bea_callspec__ shld_EvGvIb(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+SHIFT_ROTATE;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "shld ");
    #endif
    GV.ImmediatSize = 8;
    EvGv(pMyDisasm);
    GV.EIP_+= 1;
    if (!Security(0, pMyDisasm)) return;
    GV.third_arg = 1;
    (*pMyDisasm).Instruction.Immediat = *((UInt8*)(UIntPtr) (GV.EIP_- 1));
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) (*pMyDisasm).Argument3.ArgMnemonic, "%.8X",(Int64) *((UInt8*)(UIntPtr) (GV.EIP_- 1)));
    #endif
    (*pMyDisasm).Argument3.ArgType = CONSTANT_TYPE+ABSOLUTE_;
    (*pMyDisasm).Argument3.ArgSize = 8;

    FillFlags(pMyDisasm, 97);
}


/* =======================================
 *
 * ======================================= */
void __bea_callspec__ shld_EvGvCL(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+SHIFT_ROTATE;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "shld ");
    #endif
    EvGv(pMyDisasm);
    GV.third_arg = 2;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Argument3.ArgMnemonic, Registers8Bits[1]);
    #endif
    (*pMyDisasm).Argument3.ArgType = REGISTER_TYPE+GENERAL_REG+REG1;
    (*pMyDisasm).Argument3.ArgSize = 8;
    FillFlags(pMyDisasm, 97);
}

/* =======================================
 *      0fach
 * ======================================= */
void __bea_callspec__ shrd_EvGvIb(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+SHIFT_ROTATE;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "shrd ");
    #endif
    GV.ImmediatSize = 8;
    EvGv(pMyDisasm);
    GV.EIP_+= 1;
    if (!Security(0, pMyDisasm)) return;
    GV.third_arg = 1;
    (*pMyDisasm).Instruction.Immediat = *((UInt8*)(UIntPtr) (GV.EIP_- 1));
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) CopyFormattedNumber(pMyDisasm, (char*) (*pMyDisasm).Argument3.ArgMnemonic, "%.8X",(Int64) *((UInt8*)(UIntPtr) (GV.EIP_- 1)));
    #endif
    (*pMyDisasm).Argument3.ArgType = CONSTANT_TYPE+ABSOLUTE_;
    (*pMyDisasm).Argument3.ArgSize = 8;

    FillFlags(pMyDisasm, 97);
}


/* =======================================
 *
 * ======================================= */
void __bea_callspec__ shrd_EvGvCL(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+SHIFT_ROTATE;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "shrd ");
    #endif
    EvGv(pMyDisasm);
    GV.third_arg = 2;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Argument3.ArgMnemonic, Registers8Bits[1]);
    #endif
    (*pMyDisasm).Argument3.ArgType = REGISTER_TYPE+GENERAL_REG+REG1;
    (*pMyDisasm).Argument3.ArgSize = 8;
    FillFlags(pMyDisasm, 97);
}


/* =======================================
 *
 * ======================================= */
void __bea_callspec__ std_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+FLAG_CONTROL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "std ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+SPECIAL_REG+REG0;
    (*pMyDisasm).Argument1.ArgSize = 1;
    FillFlags(pMyDisasm, 99);
}


/* =======================================
 *      28h
 * ======================================= */
void __bea_callspec__ sub_EbGb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sub ");
    #endif
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,103);
}

/* =======================================
 *      29h
 * ======================================= */
void __bea_callspec__ sub_EvGv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sub ");
    #endif
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,103);
}

/* =======================================
 *      2ah
 * ======================================= */
void __bea_callspec__ sub_GbEb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sub ");
    #endif
    GbEb(pMyDisasm);
    FillFlags(pMyDisasm,103);
}

/* =======================================
 *      2bh
 * ======================================= */
void __bea_callspec__ sub_GvEv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sub ");
    #endif
    GvEv(pMyDisasm);
    FillFlags(pMyDisasm,103);
}

/* =======================================
 *      2ch
 * ======================================= */
void __bea_callspec__ sub_ALIb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sub ");
    #endif
    ALIb(pMyDisasm);
    FillFlags(pMyDisasm,103);
}

/* =======================================
 *      2dh
 * ======================================= */
void __bea_callspec__ sub_eAX_Iv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sub ");
    #endif
    eAX_Iv(pMyDisasm);
    FillFlags(pMyDisasm,103);
}

/* =======================================
 *      84h
 * ======================================= */
void __bea_callspec__ test_EbGb(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "test ");
    #endif
    (*pMyDisasm).Argument1.AccessMode = READ;
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,104);
}

/* =======================================
 *      85h
 * ======================================= */
void __bea_callspec__ test_EvGv(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "test ");
    #endif
    (*pMyDisasm).Argument1.AccessMode = READ;
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,104);
}

/* =======================================
 *      86h
 * ======================================= */
void __bea_callspec__ test_GbEb(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "test ");
    #endif
    (*pMyDisasm).Argument1.AccessMode = READ;
    GbEb(pMyDisasm);
    FillFlags(pMyDisasm,104);
}

/* =======================================
 *      87h
 * ======================================= */
void __bea_callspec__ test_GvEv(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "test ");
    #endif
    (*pMyDisasm).Argument1.AccessMode = READ;
    GvEv(pMyDisasm);
    FillFlags(pMyDisasm,104);
}

/* =======================================
 *      88h
 * ======================================= */
void __bea_callspec__ test_ALIb(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "test ");
    #endif
    (*pMyDisasm).Argument1.AccessMode = READ;
    ALIb(pMyDisasm);
    FillFlags(pMyDisasm,104);
}

/* =======================================
 *      89h
 * ======================================= */
void __bea_callspec__ test_eAX_Iv(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+BIT_UInt8;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "test ");
    #endif
    (*pMyDisasm).Argument1.AccessMode = READ;
    eAX_Iv(pMyDisasm);
    FillFlags(pMyDisasm,104);
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ ud2_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+MISCELLANEOUS_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "ud2 ");
    #endif
    GV.EIP_++;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ vmread_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmread ");
    #endif
    if (GV.Architecture == 64) {
        GV.OperandSize = 64;
    }
    EvGv(pMyDisasm);
    if (GV.Architecture == 64) {
        GV.OperandSize = 32;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ vmwrite_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmwrite ");
    #endif
    if (GV.Architecture == 64) {
        GV.OperandSize = 64;
    }
    GvEv(pMyDisasm);
    if (GV.Architecture == 64) {
        GV.OperandSize = 32;
    }
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ wbinvd_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "wbinvd ");
    #endif
    GV.EIP_++;
}

/* =======================================
 *      9Bh
 * ======================================= */
void __bea_callspec__ wait_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = FPU_INSTRUCTION+FPUCONTROL;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "wait ");
    #endif
    GV.EIP_++;
}

/* =======================================
 *
 * ======================================= */
void __bea_callspec__ wrmsr_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "wrmsr ");
    #endif
    GV.EIP_++;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG0+REG1+REG2;
    (*pMyDisasm).Argument2.ArgSize = 32;
}


/* =======================================
 *      0fc0h
 * ======================================= */
void __bea_callspec__ xadd_EbGb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xadd ");
    #endif
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,110);
}

/* =======================================
 *      0fc1h
 * ======================================= */
void __bea_callspec__ xadd_EvGv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xadd ");
    #endif
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,110);
}

/* =======================================
 *      91h
 * ======================================= */
void __bea_callspec__ xchg_ecx(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG1;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[1+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[1]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[1+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[1]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[1+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[1]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}


/* =======================================
 *      92h
 * ======================================= */
void __bea_callspec__ xchg_edx(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG2;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[2+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[2]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[2+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[2]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[2+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[2]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}


/* =======================================
 *      93h
 * ======================================= */
void __bea_callspec__ xchg_ebx(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG3;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[3+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[3]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[3+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[3]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[3+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[3]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}


/* =======================================
 *      94h
 * ======================================= */
void __bea_callspec__ xchg_esp(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG4;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[4+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[4]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[4+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[4]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[4+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[4]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}


/* =======================================
 *      95h
 * ======================================= */
void __bea_callspec__ xchg_ebp(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG5;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[5+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[5]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[5+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[5]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[5+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[5]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}


/* =======================================
 *      96h
 * ======================================= */
void __bea_callspec__ xchg_esi(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG6;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[6+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[6]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[6+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[6]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[6+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[6]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}


/* =======================================
 *      97h
 * ======================================= */
void __bea_callspec__ xchg_edi(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+DATA_TRANSFER;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
    (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG7;
    if (GV.OperandSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers64Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[7+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers64Bits[7]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 64;
        (*pMyDisasm).Argument2.ArgSize = 64;
    }
    else if (GV.OperandSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers32Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[7+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers32Bits[7]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 32;
        (*pMyDisasm).Argument2.ArgSize = 32;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Argument1.ArgMnemonic, Registers16Bits[0]);
        #endif
        if (GV.REX.B_ == 1) {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[7+8]);
            #endif
        }
        else {
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Argument2.ArgMnemonic, Registers16Bits[7]);
            #endif
        }
        GV.EIP_++;
        (*pMyDisasm).Argument2.AccessMode = WRITE;
        (*pMyDisasm).Argument1.ArgSize = 16;
        (*pMyDisasm).Argument2.ArgSize = 16;
    }
}

/* =======================================
 *      0d7h
 * ======================================= */
void __bea_callspec__ xlat_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+MISCELLANEOUS_INSTRUCTION;
    if (GV.AddressSize == 64) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xlatb ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 8;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG3;
        (*pMyDisasm).Argument2.ArgSize = 64;
        GV.EIP_++;
    }
    else if (GV.AddressSize == 32) {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xlatb ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 8;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG3;
        (*pMyDisasm).Argument2.ArgSize = 32;
        GV.EIP_++;
    }
    else {
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xlatb ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+GENERAL_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 8;
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+GENERAL_REG+REG3;
        (*pMyDisasm).Argument2.ArgSize = 16;
        GV.EIP_++;
    }
}


/* =======================================
 *      30h
 * ======================================= */
void __bea_callspec__ xor_EbGb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xor ");
    #endif
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,113);
}

/* =======================================
 *      31h
 * ======================================= */
void __bea_callspec__ xor_EvGv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xor ");
    #endif
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,113);
}

/* =======================================
 *      32h
 * ======================================= */
void __bea_callspec__ xor_GbEb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xor ");
    #endif
    GbEb(pMyDisasm);
    FillFlags(pMyDisasm,113);
}

/* =======================================
 *      33h
 * ======================================= */
void __bea_callspec__ xor_GvEv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xor ");
    #endif
    GvEv(pMyDisasm);
    FillFlags(pMyDisasm,113);
}

/* =======================================
 *      34h
 * ======================================= */
void __bea_callspec__ xor_ALIb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xor ");
    #endif
    ALIb(pMyDisasm);
    FillFlags(pMyDisasm,113);
}

/* =======================================
 *      35h
 * ======================================= */
void __bea_callspec__ xor_eAX_Iv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+LOGICAL_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xor ");
    #endif
    eAX_Iv(pMyDisasm);
    FillFlags(pMyDisasm,113);
}


/* =======================================
 *      86h
 * ======================================= */
void __bea_callspec__ xchg_EbGb(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    EbGb(pMyDisasm);
    FillFlags(pMyDisasm,113);
    (*pMyDisasm).Argument2.AccessMode = WRITE;
}

/* =======================================
 *      87h
 * ======================================= */
void __bea_callspec__ xchg_EvGv(PDISASM pMyDisasm)
{
    if ((*pMyDisasm).Prefix.LockPrefix == InvalidPrefix) {
        (*pMyDisasm).Prefix.LockPrefix = InUsePrefix;
    }
    (*pMyDisasm).Instruction.Category = GENERAL_PURPOSE_INSTRUCTION+ARITHMETIC_INSTRUCTION;
    #ifndef BEA_LIGHT_DISASSEMBLY
       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xchg ");
    #endif
    EvGv(pMyDisasm);
    FillFlags(pMyDisasm,113);
    (*pMyDisasm).Argument2.AccessMode = WRITE;
}
