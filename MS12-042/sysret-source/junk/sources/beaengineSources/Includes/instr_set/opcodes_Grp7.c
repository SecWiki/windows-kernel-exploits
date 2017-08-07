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
 *      0f01h
 * ==================================================================== */
void __bea_callspec__ G7_(PDISASM pMyDisasm)
{
    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
    GV.REGOPCODE = ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 3) & 0x7;
    GV.MOD_= ((*((UInt8*)(UIntPtr) (GV.EIP_+1))) >> 6) & 0x3;
    GV.RM_  = (*((UInt8*)(UIntPtr) (GV.EIP_+1))) & 0x7;
    if (GV.REGOPCODE == 0) {
        if (GV.MOD_== 0x3) {
            if (GV.RM_ == 0x1) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmcall ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x2) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmlaunch ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x3) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmresume ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x4) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmxoff ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else {
                FailDecode(pMyDisasm);
            }
        }
        else {
            GV.MemDecoration = Arg1fword;
            MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sgdt ");
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+MEMORY_MANAGEMENT_REG+REG0;
            (*pMyDisasm).Argument2.ArgSize = 48;
            GV.EIP_+= GV.DECALAGE_EIP+2;
        }
    }
    else if (GV.REGOPCODE == 1) {
        if (GV.MOD_== 0x3) {
            if (GV.RM_ == 0x00) {
                (*pMyDisasm).Instruction.Category = SSE3_INSTRUCTION+AGENT_SYNCHRONISATION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "monitor ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x01) {
                (*pMyDisasm).Instruction.Category = SSE3_INSTRUCTION+AGENT_SYNCHRONISATION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "mwait ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else {
                FailDecode(pMyDisasm);
            }
        }
        else {
            GV.MemDecoration = Arg1fword;
            MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
            (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "sidt ");
            #endif
            (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+MEMORY_MANAGEMENT_REG+REG2;
            (*pMyDisasm).Argument2.ArgSize = 48;
            GV.EIP_+= GV.DECALAGE_EIP+2;
        }
    }
    else if (GV.REGOPCODE == 2) {
        if (GV.MOD_== 0x3) {
            if (GV.RM_ == 0x0) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xgetbv ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x1) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "xsetbv ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else {
                FailDecode(pMyDisasm);
            }
        }
        else {
            GV.MemDecoration = Arg2fword;
            MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
            (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "lgdt ");
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+MEMORY_MANAGEMENT_REG+REG0;
            (*pMyDisasm).Argument1.ArgSize = 48;
            GV.EIP_+= GV.DECALAGE_EIP+2;
        }
    }
    else if (GV.REGOPCODE == 3) {
        if (GV.MOD_== 0x3) {
            if (GV.RM_ == 0x0) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmrun ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x1) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmmcall ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x2) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmload ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x3) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "vmsave ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x4) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "stgi ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x5) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "clgi ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x6) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "skinit ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else if (GV.RM_ == 0x7) {
                (*pMyDisasm).Instruction.Category = VM_INSTRUCTION;
                #ifndef BEA_LIGHT_DISASSEMBLY
                   (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "invlpga ");
                #endif
                GV.EIP_+= GV.DECALAGE_EIP+2;
            }
            else {
                FailDecode(pMyDisasm);
            }
        }
        else {
            GV.MemDecoration = Arg2fword;
            MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
            (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "lidt ");
            #endif
            (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+MEMORY_MANAGEMENT_REG+REG2;
            (*pMyDisasm).Argument1.ArgSize = 48;
            GV.EIP_+= GV.DECALAGE_EIP+2;
        }
    }

    else if (GV.REGOPCODE == 4) {
        GV.MemDecoration = Arg2word;
        MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
        (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "smsw ");
        #endif
        (*pMyDisasm).Argument1.ArgType = REGISTER_TYPE+CR_REG+REG0;
        (*pMyDisasm).Argument1.ArgSize = 16;
        GV.EIP_+= GV.DECALAGE_EIP+2;
    }

    else if (GV.REGOPCODE == 6) {
        GV.MemDecoration = Arg1word;
        MOD_RM(&(*pMyDisasm).Argument1, pMyDisasm);
        (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
        #ifndef BEA_LIGHT_DISASSEMBLY
           (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "lmsw ");
        #endif
        (*pMyDisasm).Argument2.ArgType = REGISTER_TYPE+CR_REG+REG0;
        (*pMyDisasm).Argument2.ArgSize = 16;
        GV.EIP_+= GV.DECALAGE_EIP+2;
    }
    else if (GV.REGOPCODE == 7) {
        if (GV.MOD_== 0x3) {
            if (GV.Architecture == 64) {
                if (GV.RM_ == 0x0) {
                    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
                    #ifndef BEA_LIGHT_DISASSEMBLY
                       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "swapgs ");
                    #endif
                    GV.EIP_+= GV.DECALAGE_EIP+2;
                }
                else {
                    FailDecode(pMyDisasm);
                }
            }
            else {
                if (GV.RM_ == 0x1) {
                    (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
                    #ifndef BEA_LIGHT_DISASSEMBLY
                       (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "rdtscp ");
                    #endif
                    GV.EIP_+= GV.DECALAGE_EIP+2;
                }
                else {
                    FailDecode(pMyDisasm);
                }
            }
        }
        else {
            GV.MemDecoration = Arg2byte;
            MOD_RM(&(*pMyDisasm).Argument2, pMyDisasm);
            (*pMyDisasm).Instruction.Category = SYSTEM_INSTRUCTION;
            #ifndef BEA_LIGHT_DISASSEMBLY
               (void) strcpy ((*pMyDisasm).Instruction.Mnemonic, "invlpg ");
            #endif
            GV.EIP_+= GV.DECALAGE_EIP+2;
        }
    }
    else {
        FailDecode(pMyDisasm);
    }


}

