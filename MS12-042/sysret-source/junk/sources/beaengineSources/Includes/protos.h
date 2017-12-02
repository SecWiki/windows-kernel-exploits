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


/* ====================================== Routines_MODRM */
void __bea_callspec__ MOD_RM(ARGTYPE*, PDISASM);
void __bea_callspec__ Reg_Opcode(ARGTYPE*, PDISASM);

void __bea_callspec__ Addr_EAX(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_ECX(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EDX(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EBX(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_SIB(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_ESI(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EDI(ARGTYPE*, PDISASM);

void __bea_callspec__ Addr_EAX_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_ECX_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EDX_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EBX_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_SIB_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EBP_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_ESI_disp8(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EDI_disp8(ARGTYPE*, PDISASM);

void __bea_callspec__ Addr_EAX_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_ECX_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EDX_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EBX_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_SIB_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EBP_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_ESI_disp32(ARGTYPE*, PDISASM);
void __bea_callspec__ Addr_EDI_disp32(ARGTYPE*, PDISASM);

void __bea_callspec__ _rEAX(ARGTYPE*, PDISASM);
void __bea_callspec__ _rECX(ARGTYPE*, PDISASM);
void __bea_callspec__ _rEDX(ARGTYPE*, PDISASM);
void __bea_callspec__ _rEBX(ARGTYPE*, PDISASM);
void __bea_callspec__ _rESP(ARGTYPE*, PDISASM);
void __bea_callspec__ _rEBP(ARGTYPE*, PDISASM);
void __bea_callspec__ _rESI(ARGTYPE*, PDISASM);
void __bea_callspec__ _rEDI(ARGTYPE*, PDISASM);

size_t __bea_callspec__ SIB_0(ARGTYPE*, size_t, PDISASM);
size_t __bea_callspec__ SIB_1(ARGTYPE*, size_t, PDISASM);
size_t __bea_callspec__ SIB_2(ARGTYPE*, size_t, PDISASM);
size_t __bea_callspec__ SIB_3(ARGTYPE*, size_t, PDISASM);

/* ====================================== Routines_Disasm */
void __bea_callspec__ CompleteInstructionFields (PDISASM);
void __bea_callspec__ EbGb(PDISASM);
void __bea_callspec__ EvGv(PDISASM);
void __bea_callspec__ EvIb(PDISASM);
void __bea_callspec__ ExGx(PDISASM);
void __bea_callspec__ EvIv(PDISASM);
void __bea_callspec__ EbIb(PDISASM);
void __bea_callspec__ Eb(PDISASM);
void __bea_callspec__ Ev(PDISASM);
void __bea_callspec__ GvEv(PDISASM);
void __bea_callspec__ GvEb(PDISASM);
void __bea_callspec__ GxEx(PDISASM);
void __bea_callspec__ GvEw(PDISASM);
void __bea_callspec__ GbEb(PDISASM);
void __bea_callspec__ ALIb(PDISASM);
void __bea_callspec__ eAX_Iv(PDISASM);
int __bea_callspec__ AnalyzeOpcode (PDISASM);
int __bea_callspec__ Security(int, PDISASM);
void __bea_callspec__ CalculateRelativeAddress(UInt64 *, Int64, PDISASM);
size_t __bea_callspec__ CopyFormattedNumber(PDISASM, char*, const char*, Int64);
void __bea_callspec__ BuildCompleteInstruction(PDISASM);
void __bea_callspec__ BuildCompleteInstructionATSyntax(PDISASM);
int __bea_callspec__ InitVariables (PDISASM);
void __bea_callspec__ FillFlags(PDISASM, int);
void __bea_callspec__ FillSegmentsRegisters (PDISASM);
void __bea_callspec__ FixArgSizeForMemoryOperand (PDISASM);
void __bea_callspec__ FixREXPrefixes (PDISASM);

/* ====================================== opcodes_prefix */
void __bea_callspec__ PrefLock(PDISASM);
void __bea_callspec__ PrefREPNE(PDISASM);
void __bea_callspec__ PrefREPE(PDISASM);
void __bea_callspec__ PrefSEGCS(PDISASM);
void __bea_callspec__ PrefSEGDS(PDISASM);
void __bea_callspec__ PrefSEGES(PDISASM);
void __bea_callspec__ PrefSEGFS(PDISASM);
void __bea_callspec__ PrefSEGGS(PDISASM);
void __bea_callspec__ PrefSEGSS(PDISASM);
void __bea_callspec__ PrefOpSize(PDISASM);
void __bea_callspec__ PrefAdSize(PDISASM);
void __bea_callspec__ Esc_2byte(PDISASM);
void __bea_callspec__ Esc_tableA4(PDISASM);
void __bea_callspec__ Esc_tableA5(PDISASM);


/* ====================================== opcodes_A_M */
void __bea_callspec__ FailDecode(PDISASM);
void __bea_callspec__ aaa_(PDISASM);
void __bea_callspec__ aad_(PDISASM);
void __bea_callspec__ aas_(PDISASM);
void __bea_callspec__ aam_(PDISASM);
void __bea_callspec__ add_EbGb(PDISASM);
void __bea_callspec__ add_EvGv(PDISASM);
void __bea_callspec__ add_GbEb(PDISASM);
void __bea_callspec__ add_GvEv(PDISASM);
void __bea_callspec__ add_ALIb(PDISASM);
void __bea_callspec__ add_eAX_Iv(PDISASM);
void __bea_callspec__ adc_ALIb(PDISASM);
void __bea_callspec__ adc_eAX_Iv(PDISASM);
void __bea_callspec__ adc_EbGb(PDISASM);
void __bea_callspec__ adc_EvGv(PDISASM);
void __bea_callspec__ adc_GbEb(PDISASM);
void __bea_callspec__ adc_GvEv(PDISASM);
void __bea_callspec__ and_EbGb(PDISASM);
void __bea_callspec__ and_ALIb(PDISASM);
void __bea_callspec__ and_eAX_Iv(PDISASM);
void __bea_callspec__ and_EvGv(PDISASM);
void __bea_callspec__ and_GbEb(PDISASM);
void __bea_callspec__ and_GvEv(PDISASM);
void __bea_callspec__ arpl_(PDISASM);
void __bea_callspec__ bound_(PDISASM);
void __bea_callspec__ bswap_eax(PDISASM);
void __bea_callspec__ bswap_ebp(PDISASM);
void __bea_callspec__ bswap_ebx(PDISASM);
void __bea_callspec__ bswap_ecx(PDISASM);
void __bea_callspec__ bswap_edi(PDISASM);
void __bea_callspec__ bswap_edx(PDISASM);
void __bea_callspec__ bswap_esi(PDISASM);
void __bea_callspec__ bswap_esp(PDISASM);
void __bea_callspec__ bsf_GvEv(PDISASM);
void __bea_callspec__ bsr_GvEv(PDISASM);
void __bea_callspec__ btc_EvGv(PDISASM);
void __bea_callspec__ btr_EvGv(PDISASM);
void __bea_callspec__ bt_EvGv(PDISASM);
void __bea_callspec__ bts_EvGv(PDISASM);
void __bea_callspec__ callf_(PDISASM);
void __bea_callspec__ call_(PDISASM);
void __bea_callspec__ cdq_(PDISASM);
void __bea_callspec__ clts_(PDISASM);
void __bea_callspec__ cmc_(PDISASM);
void __bea_callspec__ cmovo_(PDISASM);
void __bea_callspec__ cmovno_(PDISASM);
void __bea_callspec__ cmovb_(PDISASM);
void __bea_callspec__ cmovnb_(PDISASM);
void __bea_callspec__ cmove_(PDISASM);
void __bea_callspec__ cmovne_(PDISASM);
void __bea_callspec__ cmovbe_(PDISASM);
void __bea_callspec__ cmovnbe_(PDISASM);
void __bea_callspec__ cmovs_(PDISASM);
void __bea_callspec__ cmovns_(PDISASM);
void __bea_callspec__ cmovp_(PDISASM);
void __bea_callspec__ cmovnp_(PDISASM);
void __bea_callspec__ cmovl_(PDISASM);
void __bea_callspec__ cmovnl_(PDISASM);
void __bea_callspec__ cmovle_(PDISASM);
void __bea_callspec__ cmovnle_(PDISASM);
void __bea_callspec__ cmpx_EbGb(PDISASM);
void __bea_callspec__ cmpx_EvGv(PDISASM);
void __bea_callspec__ cmp_EbGb(PDISASM);
void __bea_callspec__ cmp_EvGv(PDISASM);
void __bea_callspec__ cmp_GbEb(PDISASM);
void __bea_callspec__ cmp_GvEv(PDISASM);
void __bea_callspec__ cmp_ALIb(PDISASM);
void __bea_callspec__ cmp_eAX_Iv(PDISASM);
void __bea_callspec__ cmpsb_(PDISASM);
void __bea_callspec__ cmps_(PDISASM);
void __bea_callspec__ cwde_(PDISASM);
void __bea_callspec__ clc_(PDISASM);
void __bea_callspec__ cld_(PDISASM);
void __bea_callspec__ cli_(PDISASM);
void __bea_callspec__ cpuid_(PDISASM);
void __bea_callspec__ daa_(PDISASM);
void __bea_callspec__ das_(PDISASM);
void __bea_callspec__ dec_eax(PDISASM);
void __bea_callspec__ dec_ecx(PDISASM);
void __bea_callspec__ dec_edx(PDISASM);
void __bea_callspec__ dec_ebx(PDISASM);
void __bea_callspec__ dec_esp(PDISASM);
void __bea_callspec__ dec_ebp(PDISASM);
void __bea_callspec__ dec_esi(PDISASM);
void __bea_callspec__ dec_edi(PDISASM);
void __bea_callspec__ enter_(PDISASM);
void __bea_callspec__ femms_(PDISASM);
void __bea_callspec__ hlt_(PDISASM);
void __bea_callspec__ invd_(PDISASM);
void __bea_callspec__ inc_eax(PDISASM);
void __bea_callspec__ inc_ecx(PDISASM);
void __bea_callspec__ inc_edx(PDISASM);
void __bea_callspec__ inc_ebx(PDISASM);
void __bea_callspec__ inc_esp(PDISASM);
void __bea_callspec__ inc_ebp(PDISASM);
void __bea_callspec__ inc_esi(PDISASM);
void __bea_callspec__ inc_edi(PDISASM);
void __bea_callspec__ iret_(PDISASM);
void __bea_callspec__ in_ALDX(PDISASM);
void __bea_callspec__ in_ALIb(PDISASM);
void __bea_callspec__ in_eAX_Ib(PDISASM);
void __bea_callspec__ insb_(PDISASM);
void __bea_callspec__ ins_(PDISASM);
void __bea_callspec__ into_(PDISASM);
void __bea_callspec__ in_eAX(PDISASM);
void __bea_callspec__ int_(PDISASM);
void __bea_callspec__ int1_(PDISASM);
void __bea_callspec__ int3_(PDISASM);
void __bea_callspec__ imul_GvEvIv(PDISASM);
void __bea_callspec__ imul_GvEv(PDISASM);
void __bea_callspec__ imul_GvEvIb(PDISASM);
void __bea_callspec__ jo_(PDISASM);
void __bea_callspec__ jno_(PDISASM);
void __bea_callspec__ jc_(PDISASM);
void __bea_callspec__ jnc_(PDISASM);
void __bea_callspec__ je_(PDISASM);
void __bea_callspec__ jne_(PDISASM);
void __bea_callspec__ jbe_(PDISASM);
void __bea_callspec__ jnbe_(PDISASM);
void __bea_callspec__ js_(PDISASM);
void __bea_callspec__ jns_(PDISASM);
void __bea_callspec__ jp_(PDISASM);
void __bea_callspec__ jnp_(PDISASM);
void __bea_callspec__ jl_(PDISASM);
void __bea_callspec__ jnl_(PDISASM);
void __bea_callspec__ jle_(PDISASM);
void __bea_callspec__ jnle_(PDISASM);
void __bea_callspec__ jo_near(PDISASM);
void __bea_callspec__ jno_near(PDISASM);
void __bea_callspec__ jc_near(PDISASM);
void __bea_callspec__ jnc_near(PDISASM);
void __bea_callspec__ je_near(PDISASM);
void __bea_callspec__ jne_near(PDISASM);
void __bea_callspec__ jbe_near(PDISASM);
void __bea_callspec__ ja_near(PDISASM);
void __bea_callspec__ js_near(PDISASM);
void __bea_callspec__ jns_near(PDISASM);
void __bea_callspec__ jp_near(PDISASM);
void __bea_callspec__ jnp_near(PDISASM);
void __bea_callspec__ jl_near(PDISASM);
void __bea_callspec__ jnl_near(PDISASM);
void __bea_callspec__ jle_near(PDISASM);
void __bea_callspec__ jnle_near(PDISASM);
void __bea_callspec__ jecxz_(PDISASM);
void __bea_callspec__ jmp_near(PDISASM);
void __bea_callspec__ jmp_far(PDISASM);
void __bea_callspec__ jmp_short(PDISASM);
void __bea_callspec__ lahf_(PDISASM);
void __bea_callspec__ lar_GvEw(PDISASM);
void __bea_callspec__ lds_GvM(PDISASM);
void __bea_callspec__ leave_(PDISASM);
void __bea_callspec__ lea_GvM(PDISASM);
void __bea_callspec__ les_GvM(PDISASM);
void __bea_callspec__ lodsb_(PDISASM);
void __bea_callspec__ lodsw_(PDISASM);
void __bea_callspec__ loop_(PDISASM);
void __bea_callspec__ loope_(PDISASM);
void __bea_callspec__ loopne_(PDISASM);
void __bea_callspec__ lsl_GvEw(PDISASM);
void __bea_callspec__ lss_Mp(PDISASM);
void __bea_callspec__ lfs_Mp(PDISASM);
void __bea_callspec__ lgs_Mp(PDISASM);
void __bea_callspec__ mov_RdCd(PDISASM);
void __bea_callspec__ mov_RdDd(PDISASM);
void __bea_callspec__ mov_CdRd(PDISASM);
void __bea_callspec__ mov_DdRd(PDISASM);
void __bea_callspec__ mov_EbGb(PDISASM);
void __bea_callspec__ mov_EvGv(PDISASM);
void __bea_callspec__ mov_GbEb(PDISASM);
void __bea_callspec__ mov_GvEv(PDISASM);
void __bea_callspec__ mov_ALOb(PDISASM);
void __bea_callspec__ mov_eAXOv(PDISASM);
void __bea_callspec__ mov_ObAL(PDISASM);
void __bea_callspec__ mov_OveAX(PDISASM);
void __bea_callspec__ mov_ALIb(PDISASM);
void __bea_callspec__ mov_CLIb(PDISASM);
void __bea_callspec__ mov_DLIb(PDISASM);
void __bea_callspec__ mov_BLIb(PDISASM);
void __bea_callspec__ mov_AHIb(PDISASM);
void __bea_callspec__ mov_CHIb(PDISASM);
void __bea_callspec__ mov_DHIb(PDISASM);
void __bea_callspec__ mov_BHIb(PDISASM);
void __bea_callspec__ movs_(PDISASM);
void __bea_callspec__ movsw_(PDISASM);
void __bea_callspec__ movzx_GvEb(PDISASM);
void __bea_callspec__ movsx_GvEb(PDISASM);
void __bea_callspec__ movzx_GvEw(PDISASM);
void __bea_callspec__ movsx_GvEw(PDISASM);
void __bea_callspec__ mov_EAX(PDISASM);
void __bea_callspec__ mov_ECX(PDISASM);
void __bea_callspec__ mov_EDX(PDISASM);
void __bea_callspec__ mov_EBX(PDISASM);
void __bea_callspec__ mov_ESP(PDISASM);
void __bea_callspec__ mov_EBP(PDISASM);
void __bea_callspec__ mov_ESI(PDISASM);
void __bea_callspec__ mov_EDI(PDISASM);
void __bea_callspec__ mov_EbIb(PDISASM);
void __bea_callspec__ mov_EvIv(PDISASM);
void __bea_callspec__ mov_EwSreg(PDISASM);
void __bea_callspec__ mov_SregEw(PDISASM);

/* ====================================== opcodes_N_Z */
void __bea_callspec__ nop_(PDISASM);
void __bea_callspec__ nop_Ev(PDISASM);
void __bea_callspec__ hint_nop(PDISASM);
void __bea_callspec__ or_EbGb(PDISASM);
void __bea_callspec__ or_EvGv(PDISASM);
void __bea_callspec__ or_GbEb(PDISASM);
void __bea_callspec__ or_GvEv(PDISASM);
void __bea_callspec__ or_ALIb(PDISASM);
void __bea_callspec__ or_eAX_Iv(PDISASM);
void __bea_callspec__ outsb_(PDISASM);
void __bea_callspec__ outsw_(PDISASM);
void __bea_callspec__ out_IbAL(PDISASM);
void __bea_callspec__ out_Ib_eAX(PDISASM);
void __bea_callspec__ out_DXAL(PDISASM);
void __bea_callspec__ out_DXeAX(PDISASM);
void __bea_callspec__ pop_Ev(PDISASM);
void __bea_callspec__ pop_eax(PDISASM);
void __bea_callspec__ pop_ecx(PDISASM);
void __bea_callspec__ pop_edx(PDISASM);
void __bea_callspec__ pop_ebx(PDISASM);
void __bea_callspec__ pop_esp(PDISASM);
void __bea_callspec__ pop_ebp(PDISASM);
void __bea_callspec__ pop_esi(PDISASM);
void __bea_callspec__ pop_edi(PDISASM);
void __bea_callspec__ pop_ds(PDISASM);
void __bea_callspec__ pop_es(PDISASM);
void __bea_callspec__ pop_fs(PDISASM);
void __bea_callspec__ pop_gs(PDISASM);
void __bea_callspec__ pop_ss(PDISASM);
void __bea_callspec__ popfd_(PDISASM);
void __bea_callspec__ popad_(PDISASM);
void __bea_callspec__ push_eax(PDISASM);
void __bea_callspec__ push_ecx(PDISASM);
void __bea_callspec__ push_edx(PDISASM);
void __bea_callspec__ push_ebx(PDISASM);
void __bea_callspec__ push_esp(PDISASM);
void __bea_callspec__ push_ebp(PDISASM);
void __bea_callspec__ push_esi(PDISASM);
void __bea_callspec__ push_edi(PDISASM);
void __bea_callspec__ push_cs(PDISASM);
void __bea_callspec__ push_ds(PDISASM);
void __bea_callspec__ push_es(PDISASM);
void __bea_callspec__ push_fs(PDISASM);
void __bea_callspec__ push_gs(PDISASM);
void __bea_callspec__ push_ss(PDISASM);
void __bea_callspec__ pushfd_(PDISASM);
void __bea_callspec__ pushad_(PDISASM);
void __bea_callspec__ push_Iv(PDISASM);
void __bea_callspec__ push_Ib(PDISASM);
void __bea_callspec__ pushfd_(PDISASM);
void __bea_callspec__ pushad_(PDISASM);
void __bea_callspec__ retn_(PDISASM);
void __bea_callspec__ ret_(PDISASM);
void __bea_callspec__ retf_(PDISASM);
void __bea_callspec__ retf_Iw(PDISASM);
void __bea_callspec__ rdtsc_(PDISASM);
void __bea_callspec__ rdmsr_(PDISASM);
void __bea_callspec__ rdpmc_(PDISASM);
void __bea_callspec__ rsm_(PDISASM);
void __bea_callspec__ sysenter_(PDISASM);
void __bea_callspec__ sysexit_(PDISASM);
void __bea_callspec__ sahf_(PDISASM);
void __bea_callspec__ salc_(PDISASM);
void __bea_callspec__ scasb_(PDISASM);
void __bea_callspec__ scas_(PDISASM);
void __bea_callspec__ stc_(PDISASM);
void __bea_callspec__ sti_(PDISASM);
void __bea_callspec__ stos_(PDISASM);
void __bea_callspec__ stosw_(PDISASM);
void __bea_callspec__ syscall_(PDISASM);
void __bea_callspec__ sysret_(PDISASM);
void __bea_callspec__ sbb_EbGb(PDISASM);
void __bea_callspec__ sbb_EvGv(PDISASM);
void __bea_callspec__ sbb_GbEb(PDISASM);
void __bea_callspec__ sbb_GvEv(PDISASM);
void __bea_callspec__ sbb_ALIb(PDISASM);
void __bea_callspec__ sbb_eAX_Iv(PDISASM);
void __bea_callspec__ seto_(PDISASM);
void __bea_callspec__ setno_(PDISASM);
void __bea_callspec__ setb_(PDISASM);
void __bea_callspec__ setnb_(PDISASM);
void __bea_callspec__ sete_(PDISASM);
void __bea_callspec__ setne_(PDISASM);
void __bea_callspec__ setbe_(PDISASM);
void __bea_callspec__ setnbe_(PDISASM);
void __bea_callspec__ sets_(PDISASM);
void __bea_callspec__ setns_(PDISASM);
void __bea_callspec__ setp_(PDISASM);
void __bea_callspec__ setnp_(PDISASM);
void __bea_callspec__ setnge_(PDISASM);
void __bea_callspec__ setge_(PDISASM);
void __bea_callspec__ setle_(PDISASM);
void __bea_callspec__ setnle_(PDISASM);
void __bea_callspec__ shld_EvGvIb(PDISASM);
void __bea_callspec__ shld_EvGvCL(PDISASM);
void __bea_callspec__ shrd_EvGvIb(PDISASM);
void __bea_callspec__ shrd_EvGvCL(PDISASM);
void __bea_callspec__ std_(PDISASM);
void __bea_callspec__ sub_ALIb(PDISASM);
void __bea_callspec__ sub_eAX_Iv(PDISASM);
void __bea_callspec__ sub_EbGb(PDISASM);
void __bea_callspec__ sub_EvGv(PDISASM);
void __bea_callspec__ sub_GbEb(PDISASM);
void __bea_callspec__ sub_GvEv(PDISASM);
void __bea_callspec__ test_ALIb(PDISASM);
void __bea_callspec__ test_eAX_Iv(PDISASM);
void __bea_callspec__ test_EbGb(PDISASM);
void __bea_callspec__ test_EvGv(PDISASM);
void __bea_callspec__ test_GbEb(PDISASM);
void __bea_callspec__ test_GvEv(PDISASM);
void __bea_callspec__ ud2_(PDISASM);
void __bea_callspec__ vmread_(PDISASM);
void __bea_callspec__ vmwrite_(PDISASM);
void __bea_callspec__ wbinvd_(PDISASM);
void __bea_callspec__ wait_(PDISASM);
void __bea_callspec__ wrmsr_(PDISASM);
void __bea_callspec__ xadd_EbGb(PDISASM);
void __bea_callspec__ xadd_EvGv(PDISASM);
void __bea_callspec__ xchg_EbGb(PDISASM);
void __bea_callspec__ xchg_ebp(PDISASM);
void __bea_callspec__ xchg_ebx(PDISASM);
void __bea_callspec__ xchg_ecx(PDISASM);
void __bea_callspec__ xchg_edi(PDISASM);
void __bea_callspec__ xchg_edx(PDISASM);
void __bea_callspec__ xchg_esi(PDISASM);
void __bea_callspec__ xchg_esp(PDISASM);
void __bea_callspec__ xchg_EvGv(PDISASM);
void __bea_callspec__ xlat_(PDISASM);
void __bea_callspec__ xor_ALIb(PDISASM);
void __bea_callspec__ xor_eAX_Iv(PDISASM);
void __bea_callspec__ xor_EbGb(PDISASM);
void __bea_callspec__ xor_EvGv(PDISASM);
void __bea_callspec__ xor_GbEb(PDISASM);
void __bea_callspec__ xor_GvEv(PDISASM);

/* ====================================== opcodes_Grp1 */
void __bea_callspec__ G1_EbIb(PDISASM);
void __bea_callspec__ G1_EbIb2(PDISASM);
void __bea_callspec__ G1_EvIv(PDISASM);
void __bea_callspec__ G1_EvIb(PDISASM);

/* ====================================== opcodes_Grp2 */
void __bea_callspec__ G2_EbIb(PDISASM);
void __bea_callspec__ G2_EvIb(PDISASM);
void __bea_callspec__ G2_Ev1(PDISASM);
void __bea_callspec__ G2_Eb1(PDISASM);
void __bea_callspec__ G2_EbCL(PDISASM);
void __bea_callspec__ G2_EvCL(PDISASM);

/* ====================================== opcodes_Grp3 */
void __bea_callspec__ G3_Eb(PDISASM);
void __bea_callspec__ G3_Ev(PDISASM);

/* ====================================== opcodes_Grp4 */
void __bea_callspec__ G4_Eb(PDISASM);

/* ====================================== opcodes_Grp5 */
void __bea_callspec__ G5_Ev(PDISASM);

/* ====================================== opcodes_Grp6 */
void __bea_callspec__ G6_(PDISASM);

/* ====================================== opcodes_Grp7 */
void __bea_callspec__ G7_(PDISASM);

/* ====================================== opcodes_Grp8 */
void __bea_callspec__ G8_EvIb(PDISASM);

/* ====================================== opcodes_Grp9 */
void __bea_callspec__ G9_(PDISASM);

/* ====================================== opcodes_Grp12 */
void __bea_callspec__ G12_(PDISASM);

/* ====================================== opcodes_Grp13 */
void __bea_callspec__ G13_(PDISASM);

/* ====================================== opcodes_Grp14 */
void __bea_callspec__ G14_(PDISASM);

/* ====================================== opcodes_Grp15 */
void __bea_callspec__ G15_(PDISASM);

/* ====================================== opcodes_Grp16 */
void __bea_callspec__ G16_(PDISASM);

/* ====================================== opcodes_FPU */
void __bea_callspec__ D8_(PDISASM);
void __bea_callspec__ D9_(PDISASM);
void __bea_callspec__ DA_(PDISASM);
void __bea_callspec__ DB_(PDISASM);
void __bea_callspec__ DC_(PDISASM);
void __bea_callspec__ DD_(PDISASM);
void __bea_callspec__ DE_(PDISASM);
void __bea_callspec__ DF_(PDISASM);

/* ====================================== opcodes_MMX */
void __bea_callspec__ emms_(PDISASM);
void __bea_callspec__ movd_EP(PDISASM);
void __bea_callspec__ movd_PE(PDISASM);
void __bea_callspec__ movq_PQ(PDISASM);
void __bea_callspec__ movq_QP(PDISASM);
void __bea_callspec__ movq_WV(PDISASM);
void __bea_callspec__ pabsb_(PDISASM);
void __bea_callspec__ pabsd_(PDISASM);
void __bea_callspec__ pabsw_(PDISASM);
void __bea_callspec__ packssdw_(PDISASM);
void __bea_callspec__ packsswb_(PDISASM);
void __bea_callspec__ packuswb_(PDISASM);
void __bea_callspec__ paddb_(PDISASM);
void __bea_callspec__ paddd_(PDISASM);
void __bea_callspec__ paddsb_(PDISASM);
void __bea_callspec__ paddsw_(PDISASM);
void __bea_callspec__ paddusb_(PDISASM);
void __bea_callspec__ paddusw_(PDISASM);
void __bea_callspec__ paddw_(PDISASM);
void __bea_callspec__ pandn_(PDISASM);
void __bea_callspec__ pand_(PDISASM);
void __bea_callspec__ pcmpeqb_(PDISASM);
void __bea_callspec__ pcmpeqd_(PDISASM);
void __bea_callspec__ pcmpeqw_(PDISASM);
void __bea_callspec__ pcmpgtb_(PDISASM);
void __bea_callspec__ pcmpgtd_(PDISASM);
void __bea_callspec__ pcmpgtw_(PDISASM);
void __bea_callspec__ pmulhw_(PDISASM);
void __bea_callspec__ pmullw_(PDISASM);
void __bea_callspec__ pmaddwd_(PDISASM);
void __bea_callspec__ por_(PDISASM);
void __bea_callspec__ pslld_(PDISASM);
void __bea_callspec__ psllq_(PDISASM);
void __bea_callspec__ psllw_(PDISASM);
void __bea_callspec__ psrld_(PDISASM);
void __bea_callspec__ psrlq_(PDISASM);
void __bea_callspec__ psrlw_(PDISASM);
void __bea_callspec__ psrad_(PDISASM);
void __bea_callspec__ psraw_(PDISASM);
void __bea_callspec__ psubb_(PDISASM);
void __bea_callspec__ psubd_(PDISASM);
void __bea_callspec__ psubsb_(PDISASM);
void __bea_callspec__ psubsw_(PDISASM);
void __bea_callspec__ psubusb_(PDISASM);
void __bea_callspec__ psubusw_(PDISASM);
void __bea_callspec__ psubw_(PDISASM);
void __bea_callspec__ punpckhbw_(PDISASM);
void __bea_callspec__ punpckhdq_(PDISASM);
void __bea_callspec__ punpckhwd_(PDISASM);
void __bea_callspec__ punpcklbw_(PDISASM);
void __bea_callspec__ punpckldq_(PDISASM);
void __bea_callspec__ punpcklwd_(PDISASM);
void __bea_callspec__ pxor_(PDISASM);

/* ====================================== opcodes_SSE */
void __bea_callspec__ addps_VW(PDISASM);
void __bea_callspec__ addsubpd_(PDISASM);
void __bea_callspec__ andnps_VW(PDISASM);
void __bea_callspec__ andps_VW(PDISASM);
void __bea_callspec__ blendpd_(PDISASM);
void __bea_callspec__ blendps_(PDISASM);
void __bea_callspec__ blendvpd_(PDISASM);
void __bea_callspec__ blendvps_(PDISASM);
void __bea_callspec__ cmpps_VW(PDISASM);
void __bea_callspec__ crc32_GvEb(PDISASM);
void __bea_callspec__ crc32_GvEv(PDISASM);
void __bea_callspec__ comiss_VW(PDISASM);
void __bea_callspec__ cvtdq2ps_(PDISASM);
void __bea_callspec__ cvtpd2dq_(PDISASM);
void __bea_callspec__ cvtpi2ps_(PDISASM);
void __bea_callspec__ cvtps2pd_(PDISASM);
void __bea_callspec__ cvtps2pi_(PDISASM);
void __bea_callspec__ cvttps2pi_(PDISASM);
void __bea_callspec__ dppd_(PDISASM);
void __bea_callspec__ dpps_(PDISASM);
void __bea_callspec__ divps_VW(PDISASM);
void __bea_callspec__ extractps_(PDISASM);
void __bea_callspec__ haddpd_VW(PDISASM);
void __bea_callspec__ hsubpd_VW(PDISASM);
void __bea_callspec__ insertps_(PDISASM);
void __bea_callspec__ lddqu_(PDISASM);
void __bea_callspec__ maskmovq_(PDISASM);
void __bea_callspec__ maxps_VW(PDISASM);
void __bea_callspec__ minps_VW(PDISASM);
void __bea_callspec__ movaps_VW(PDISASM);
void __bea_callspec__ movaps_WV(PDISASM);
void __bea_callspec__ movhps_MV(PDISASM);
void __bea_callspec__ movhps_VM(PDISASM);
void __bea_callspec__ movlps_MV(PDISASM);
void __bea_callspec__ movlps_VM(PDISASM);
void __bea_callspec__ movmskps_(PDISASM);
void __bea_callspec__ movntdqa_(PDISASM);
void __bea_callspec__ movnti_(PDISASM);
void __bea_callspec__ movntps_(PDISASM);
void __bea_callspec__ movntq_(PDISASM);
void __bea_callspec__ movups_VW(PDISASM);
void __bea_callspec__ movups_WV(PDISASM);
void __bea_callspec__ mpsadbw_(PDISASM);
void __bea_callspec__ mulps_VW(PDISASM);
void __bea_callspec__ orps_VW(PDISASM);
void __bea_callspec__ packusdw_(PDISASM);
void __bea_callspec__ paddq_(PDISASM);
void __bea_callspec__ pavgb_(PDISASM);
void __bea_callspec__ pavgw_(PDISASM);
void __bea_callspec__ palignr_(PDISASM);
void __bea_callspec__ pblendvb_(PDISASM);
void __bea_callspec__ pblendw_(PDISASM);
void __bea_callspec__ pcmpeqq_(PDISASM);
void __bea_callspec__ pcmpestri_(PDISASM);
void __bea_callspec__ pcmpestrm_(PDISASM);
void __bea_callspec__ pcmpgtq_(PDISASM);
void __bea_callspec__ pcmpistri_(PDISASM);
void __bea_callspec__ pcmpistrm_(PDISASM);
void __bea_callspec__ pextrb_(PDISASM);
void __bea_callspec__ pextrd_(PDISASM);
void __bea_callspec__ pextrw2_(PDISASM);
void __bea_callspec__ pextrw_(PDISASM);
void __bea_callspec__ phaddd_(PDISASM);
void __bea_callspec__ phaddsw_(PDISASM);
void __bea_callspec__ phaddw_(PDISASM);
void __bea_callspec__ phminposuw_(PDISASM);
void __bea_callspec__ phsubd_(PDISASM);
void __bea_callspec__ phsubsw_(PDISASM);
void __bea_callspec__ phsubw_(PDISASM);
void __bea_callspec__ pinsrb_(PDISASM);
void __bea_callspec__ pinsrd_(PDISASM);
void __bea_callspec__ pinsrw_(PDISASM);
void __bea_callspec__ pmaxsb_(PDISASM);
void __bea_callspec__ pmaxsd_(PDISASM);
void __bea_callspec__ pmaxsw_(PDISASM);
void __bea_callspec__ pmaxub_(PDISASM);
void __bea_callspec__ pmaxud_(PDISASM);
void __bea_callspec__ pmaxuw_(PDISASM);
void __bea_callspec__ pminsb_(PDISASM);
void __bea_callspec__ pminsd_(PDISASM);
void __bea_callspec__ pminsw_(PDISASM);
void __bea_callspec__ pminub_(PDISASM);
void __bea_callspec__ pminud_(PDISASM);
void __bea_callspec__ pminuw_(PDISASM);
void __bea_callspec__ pmaddubsw_(PDISASM);
void __bea_callspec__ pmovmskb_(PDISASM);
void __bea_callspec__ pmovsxbd_(PDISASM);
void __bea_callspec__ pmovsxbq_(PDISASM);
void __bea_callspec__ pmovsxbw_(PDISASM);
void __bea_callspec__ pmovsxdq_(PDISASM);
void __bea_callspec__ pmovsxwd_(PDISASM);
void __bea_callspec__ pmovsxwq_(PDISASM);
void __bea_callspec__ pmovzxbd_(PDISASM);
void __bea_callspec__ pmovzxbq_(PDISASM);
void __bea_callspec__ pmovzxbw_(PDISASM);
void __bea_callspec__ pmovzxdq_(PDISASM);
void __bea_callspec__ pmovzxwd_(PDISASM);
void __bea_callspec__ pmovzxwq_(PDISASM);
void __bea_callspec__ pmuldq_(PDISASM);
void __bea_callspec__ pmulhrsw_(PDISASM);
void __bea_callspec__ pmulhuw_(PDISASM);
void __bea_callspec__ pmulhw_(PDISASM);
void __bea_callspec__ pmulld_(PDISASM);
void __bea_callspec__ pmullw_(PDISASM);
void __bea_callspec__ pmuludq_(PDISASM);
void __bea_callspec__ popcnt_(PDISASM);
void __bea_callspec__ psadbw_(PDISASM);
void __bea_callspec__ pshufb_(PDISASM);
void __bea_callspec__ pshufw_(PDISASM);
void __bea_callspec__ psignb_(PDISASM);
void __bea_callspec__ psignd_(PDISASM);
void __bea_callspec__ psignw_(PDISASM);
void __bea_callspec__ psubq_(PDISASM);
void __bea_callspec__ ptest_(PDISASM);
void __bea_callspec__ punpcklqdq_(PDISASM);
void __bea_callspec__ punpckhqdq_(PDISASM);
void __bea_callspec__ rcpps_(PDISASM);
void __bea_callspec__ roundpd_(PDISASM);
void __bea_callspec__ roundps_(PDISASM);
void __bea_callspec__ roundsd_(PDISASM);
void __bea_callspec__ roundss_(PDISASM);
void __bea_callspec__ rsqrtps_(PDISASM);
void __bea_callspec__ shufps_(PDISASM);
void __bea_callspec__ sqrtps_VW(PDISASM);
void __bea_callspec__ subps_VW(PDISASM);
void __bea_callspec__ ucomiss_VW(PDISASM);
void __bea_callspec__ unpckhps_(PDISASM);
void __bea_callspec__ unpcklps_(PDISASM);
void __bea_callspec__ xorps_VW(PDISASM);

/* ====================================== opcodes_AES */
void __bea_callspec__ aesimc(PDISASM);
void __bea_callspec__ aesdec(PDISASM);
void __bea_callspec__ aesdeclast(PDISASM);
void __bea_callspec__ aesenc(PDISASM);
void __bea_callspec__ aesenclast(PDISASM);
void __bea_callspec__ aeskeygen(PDISASM);

/* ====================================== opcodes_CLMUL */
void __bea_callspec__ pclmulqdq_(PDISASM);
