// $Id$

#ifndef CPUCORE_HH
#define CPUCORE_HH

#include "openmsx.hh"
#include "Observer.hh"
#include "CPU.hh"
#include "CacheLine.hh"
#include <string>
#include <memory>

namespace openmsx {

class MSXCPUInterface;
class Scheduler;
class MSXMotherBoard;
class EmuTime;
class BooleanSetting;
class IntegerSetting;
class Setting;

typedef signed char offset;

template <class CPU_POLICY>
class CPUCore : private CPU_POLICY, public CPU, private Observer<Setting>
{
public:
	typedef void (CPUCore::*FuncPtr)();

	CPUCore(MSXMotherBoard& motherboard, const std::string& name,
	        const BooleanSetting& traceSetting, const EmuTime& time);
	virtual ~CPUCore();

	void setInterface(MSXCPUInterface* interf);

	/**
	 * Reset the CPU.
	 */
	void doReset(const EmuTime& time);

	virtual void execute();
	virtual void exitCPULoopSync();
	virtual void exitCPULoopAsync();
	virtual void warp(const EmuTime& time);
	virtual const EmuTime& getCurrentTime() const;
	virtual void wait(const EmuTime& time);
	virtual void invalidateMemCache(word start, unsigned size);
	virtual CPURegs& getRegisters();
	virtual void doStep();
	virtual void doContinue();
	virtual void doBreak();

	virtual void disasmCommand(const std::vector<TclObject*>& tokens,
                                   TclObject& result) const;

	/**
	 * Read a byte from memory. If possible the byte is read from
	 * cache, otherwise the readMem() method of MSXCPUInterface is used.
	 */
	byte readMem(word address);

	/**
	 * Write a byte from memory. If possible the byte is written to
	 * cache, otherwise the writeMem() method of MSXCPUInterface is used.
	 */
	void writeMem(word address, byte value);

	/**
	 * Raises the maskable interrupt count.
	 * Devices should call MSXCPU::raiseIRQ instead, or use the IRQHelper class.
	 */
	void raiseIRQ();

	/**
	 * Lowers the maskable interrupt count.
	 * Devices should call MSXCPU::lowerIRQ instead, or use the IRQHelper class.
	 */
	void lowerIRQ();

	/**
	 * Raises the non-maskable interrupt count.
	 * Devices should call MSXCPU::raiseNMI instead, or use the IRQHelper class.
	 */
	void raiseNMI();

	/**
	 * Lowers the non-maskable interrupt count.
	 * Devices should call MSXCPU::lowerNMI instead, or use the IRQHelper class.
	 */
	void lowerNMI();

	/**
	 * Change the clock freq.
	 */
	void setFreq(unsigned freq);

private:
	void doContinue2();
	bool needExitCPULoop();
	void setSlowInstructions();

	// Observer<Setting>
	virtual void update(const Setting& setting);

	MSXMotherBoard& motherboard;
	Scheduler& scheduler;
	MSXCPUInterface* interface;

	// memory cache
	const byte* readCacheLine[CacheLine::NUM];
	byte* writeCacheLine[CacheLine::NUM];
	bool readCacheTried [CacheLine::NUM];
	bool writeCacheTried[CacheLine::NUM];

	// dynamic freq
	std::auto_ptr<BooleanSetting> freqLocked;
	std::auto_ptr<IntegerSetting> freqValue;
	unsigned freq;

	const BooleanSetting& traceSetting;

	// state machine variables
	int slowInstructions;
	int NMIStatus;
	int IRQStatus;
	CPURegs R;

	word memptr;
	offset ofst;

	/**
	 * Set to true when there was a rising edge on the NMI line
	 * (rising = non-active -> active).
	 * Set to false when the CPU jumps to the NMI handler address.
	 */
	bool nmiEdge;

	volatile bool exitLoop;


	inline void cpuTracePre();
	inline void cpuTracePost();

	inline byte READ_PORT(word port);
	inline void WRITE_PORT(word port, byte value);
	inline byte RDMEM_common(word address);
	byte RDMEMslow(word address);
	inline void WRMEM_common(word address, byte value);
	void WRMEMslow(word address, byte value);
	inline byte RDMEM_OPCODE();
	inline word RD_WORD_PC();
	word RD_WORD_PC_slow();
	inline word RD_WORD(word address);
	word RD_WORD_slow(word address);
	inline byte RDMEM(word address);
	inline void WRMEM(word address, byte value);
	inline void WR_WORD(word address, word value);
	void WR_WORD_slow(word address, word value);
	inline void WR_WORD_rev(word address, word value);
	void WR_WORD_rev_slow(word address, word value);

	inline void M1Cycle();
	inline void executeInternal();
	inline void executeInstruction();
	inline void executeInstruction1(byte opcode);
	inline void nmi();
	inline void irq0();
	inline void irq1();
	inline void irq2();
	inline void executeFast();
	void executeSlow();
	inline void next();

	inline bool C();
	inline bool NC();
	inline bool Z();
	inline bool NZ();
	inline bool M();
	inline bool P();
	inline bool PE();
	inline bool PO();

	static const FuncPtr opcode_dd_cb[256];
	static const FuncPtr opcode_fd_cb[256];
	static const FuncPtr opcode_cb[256];
	static const FuncPtr opcode_ed[256];
	static const FuncPtr opcode_dd[256];
	static const FuncPtr opcode_fd[256];
	static const FuncPtr opcode_main[256];

	void ld_a_a();
	void ld_a_b();
	void ld_a_c();
	void ld_a_d();
	void ld_a_e();
	void ld_a_h();
	void ld_a_l();
	void ld_a_ixh();
	void ld_a_ixl();
	void ld_a_iyh();
	void ld_a_iyl();
	void ld_b_b();
	void ld_b_a();
	void ld_b_c();
	void ld_b_d();
	void ld_b_e();
	void ld_b_h();
	void ld_b_l();
	void ld_b_ixh();
	void ld_b_ixl();
	void ld_b_iyh();
	void ld_b_iyl();
	void ld_c_c();
	void ld_c_a();
	void ld_c_b();
	void ld_c_d();
	void ld_c_e();
	void ld_c_h();
	void ld_c_l();
	void ld_c_ixh();
	void ld_c_ixl();
	void ld_c_iyh();
	void ld_c_iyl();
	void ld_d_d();
	void ld_d_a();
	void ld_d_c();
	void ld_d_b();
	void ld_d_e();
	void ld_d_h();
	void ld_d_l();
	void ld_d_ixh();
	void ld_d_ixl();
	void ld_d_iyh();
	void ld_d_iyl();
	void ld_e_e();
	void ld_e_a();
	void ld_e_c();
	void ld_e_b();
	void ld_e_d();
	void ld_e_h();
	void ld_e_l();
	void ld_e_ixh();
	void ld_e_ixl();
	void ld_e_iyh();
	void ld_e_iyl();
	void ld_h_h();
	void ld_h_a();
	void ld_h_c();
	void ld_h_b();
	void ld_h_e();
	void ld_h_d();
	void ld_h_l();
	void ld_l_l();
	void ld_l_a();
	void ld_l_c();
	void ld_l_b();
	void ld_l_e();
	void ld_l_d();
	void ld_l_h();
	void ld_ixh_a();
	void ld_ixh_b();
	void ld_ixh_c();
	void ld_ixh_d();
	void ld_ixh_e();
	void ld_ixh_ixh();
	void ld_ixh_ixl();
	void ld_ixl_a();
	void ld_ixl_b();
	void ld_ixl_c();
	void ld_ixl_d();
	void ld_ixl_e();
	void ld_ixl_ixh();
	void ld_ixl_ixl();
	void ld_iyh_a();
	void ld_iyh_b();
	void ld_iyh_c();
	void ld_iyh_d();
	void ld_iyh_e();
	void ld_iyh_iyh();
	void ld_iyh_iyl();
	void ld_iyl_a();
	void ld_iyl_b();
	void ld_iyl_c();
	void ld_iyl_d();
	void ld_iyl_e();
	void ld_iyl_iyh();
	void ld_iyl_iyl();

	void ld_sp_hl();
	void ld_sp_ix();
	void ld_sp_iy();

	inline void WR_X_A(word x);
	void ld_xbc_a();
	void ld_xde_a();
	void ld_xhl_a();

	inline void WR_HL_X(byte val);
	void ld_xhl_b();
	void ld_xhl_c();
	void ld_xhl_d();
	void ld_xhl_e();
	void ld_xhl_h();
	void ld_xhl_l();

	void ld_xhl_byte();
	inline void WR_X_X(byte val);
	inline void WR_XIX(byte val);
	void ld_xix_a();
	void ld_xix_b();
	void ld_xix_c();
	void ld_xix_d();
	void ld_xix_e();
	void ld_xix_h();
	void ld_xix_l();

	inline void WR_XIY(byte val);
	void ld_xiy_a();
	void ld_xiy_b();
	void ld_xiy_c();
	void ld_xiy_d();
	void ld_xiy_e();
	void ld_xiy_h();
	void ld_xiy_l();

	void ld_xix_byte();
	void ld_xiy_byte();

	void ld_xbyte_a();

	inline void WR_NN_Y(word reg);
	void ld_xword_bc();
	void ld_xword_de();
	void ld_xword_hl();
	void ld_xword_ix();
	void ld_xword_iy();
	void ld_xword_sp();

	inline void RD_A_X(word x);
	void ld_a_xbc();
	void ld_a_xde();
	void ld_a_xhl();

	void ld_a_xix();
	void ld_a_xiy();

	void ld_a_xbyte();

	void ld_a_byte();
	void ld_b_byte();
	void ld_c_byte();
	void ld_d_byte();
	void ld_e_byte();
	void ld_h_byte();
	void ld_l_byte();
	void ld_ixh_byte();
	void ld_ixl_byte();
	void ld_iyh_byte();
	void ld_iyl_byte();

	void ld_b_xhl();
	void ld_c_xhl();
	void ld_d_xhl();
	void ld_e_xhl();
	void ld_h_xhl();
	void ld_l_xhl();

	inline byte RD_R_XIX();
	void ld_b_xix();
	void ld_c_xix();
	void ld_d_xix();
	void ld_e_xix();
	void ld_h_xix();
	void ld_l_xix();

	inline byte RD_R_XIY();
	void ld_b_xiy();
	void ld_c_xiy();
	void ld_d_xiy();
	void ld_e_xiy();
	void ld_h_xiy();
	void ld_l_xiy();

	inline word RD_P_XX();
	void ld_bc_xword();
	void ld_de_xword();
	void ld_hl_xword();
	void ld_ix_xword();
	void ld_iy_xword();
	void ld_sp_xword();

	void ld_bc_word();
	void ld_de_word();
	void ld_hl_word();
	void ld_ix_word();
	void ld_iy_word();
	void ld_sp_word();

	inline void ADC(byte reg);
	void adc_a_a();
	void adc_a_b();
	void adc_a_c();
	void adc_a_d();
	void adc_a_e();
	void adc_a_h();
	void adc_a_l();
	void adc_a_ixl();
	void adc_a_ixh();
	void adc_a_iyl();
	void adc_a_iyh();
	void adc_a_byte();
	inline void adc_a_x(word x);
	void adc_a_xhl();
	void adc_a_xix();
	void adc_a_xiy();

	inline void ADD(byte reg);
	void add_a_a();
	void add_a_b();
	void add_a_c();
	void add_a_d();
	void add_a_e();
	void add_a_h();
	void add_a_l();
	void add_a_ixl();
	void add_a_ixh();
	void add_a_iyl();
	void add_a_iyh();
	void add_a_byte();
	inline void add_a_x(word x);
	void add_a_xhl();
	void add_a_xix();
	void add_a_xiy();

	inline void AND(byte reg);
	void and_a();
	void and_b();
	void and_c();
	void and_d();
	void and_e();
	void and_h();
	void and_l();
	void and_ixh();
	void and_ixl();
	void and_iyh();
	void and_iyl();
	void and_byte();
	inline void and_x(word x);
	void and_xhl();
	void and_xix();
	void and_xiy();

	inline void CP(byte reg);
	void cp_a();
	void cp_b();
	void cp_c();
	void cp_d();
	void cp_e();
	void cp_h();
	void cp_l();
	void cp_ixh();
	void cp_ixl();
	void cp_iyh();
	void cp_iyl();
	void cp_byte();
	inline void cp_x(word x);
	void cp_xhl();
	void cp_xix();
	void cp_xiy();

	inline void OR(byte reg);
	void or_a();
	void or_b();
	void or_c();
	void or_d();
	void or_e();
	void or_h();
	void or_l();
	void or_ixh();
	void or_ixl();
	void or_iyh();
	void or_iyl();
	void or_byte();
	inline void or_x(word x);
	void or_xhl();
	void or_xix();
	void or_xiy();

	inline void SBC(byte reg);
	void sbc_a_a();
	void sbc_a_b();
	void sbc_a_c();
	void sbc_a_d();
	void sbc_a_e();
	void sbc_a_h();
	void sbc_a_l();
	void sbc_a_ixh();
	void sbc_a_ixl();
	void sbc_a_iyh();
	void sbc_a_iyl();
	void sbc_a_byte();
	inline void sbc_a_x(word x);
	void sbc_a_xhl();
	void sbc_a_xix();
	void sbc_a_xiy();

	inline void SUB(byte reg);
	void sub_a();
	void sub_b();
	void sub_c();
	void sub_d();
	void sub_e();
	void sub_h();
	void sub_l();
	void sub_ixh();
	void sub_ixl();
	void sub_iyh();
	void sub_iyl();
	void sub_byte();
	inline void sub_x(word x);
	void sub_xhl();
	void sub_xix();
	void sub_xiy();

	inline void XOR(byte reg);
	void xor_a();
	void xor_b();
	void xor_c();
	void xor_d();
	void xor_e();
	void xor_h();
	void xor_l();
	void xor_ixh();
	void xor_ixl();
	void xor_iyh();
	void xor_iyl();
	void xor_byte();
	inline void xor_x(word x);
	void xor_xhl();
	void xor_xix();
	void xor_xiy();

	inline byte DEC(byte reg);
	void dec_a();
	void dec_b();
	void dec_c();
	void dec_d();
	void dec_e();
	void dec_h();
	void dec_l();
	void dec_ixh();
	void dec_ixl();
	void dec_iyh();
	void dec_iyl();
	inline void DEC_X(word x);
	void dec_xhl();
	void dec_xix();
	void dec_xiy();

	inline byte INC(byte reg);
	void inc_a();
	void inc_b();
	void inc_c();
	void inc_d();
	void inc_e();
	void inc_h();
	void inc_l();
	void inc_ixh();
	void inc_ixl();
	void inc_iyh();
	void inc_iyl();
	inline void INC_X(word x);
	void inc_xhl();
	void inc_xix();
	void inc_xiy();

	inline void ADCW(word reg);
	void adc_hl_bc();
	void adc_hl_de();
	void adc_hl_hl();
	void adc_hl_sp();

	inline word ADDW(word reg1, word reg2);
	void add_hl_bc();
	void add_hl_de();
	void add_hl_hl();
	void add_hl_sp();
	void add_ix_bc();
	void add_ix_de();
	void add_ix_ix();
	void add_ix_sp();
	void add_iy_bc();
	void add_iy_de();
	void add_iy_iy();
	void add_iy_sp();

	inline void SBCW(word reg);
	void sbc_hl_bc();
	void sbc_hl_de();
	void sbc_hl_hl();
	void sbc_hl_sp();

	void dec_bc();
	void dec_de();
	void dec_hl();
	void dec_ix();
	void dec_iy();
	void dec_sp();

	void inc_bc();
	void inc_de();
	void inc_hl();
	void inc_ix();
	void inc_iy();
	void inc_sp();

	inline void BIT(byte bit, byte reg);
	void bit_0_a();
	void bit_0_b();
	void bit_0_c();
	void bit_0_d();
	void bit_0_e();
	void bit_0_h();
	void bit_0_l();
	void bit_1_a();
	void bit_1_b();
	void bit_1_c();
	void bit_1_d();
	void bit_1_e();
	void bit_1_h();
	void bit_1_l();
	void bit_2_a();
	void bit_2_b();
	void bit_2_c();
	void bit_2_d();
	void bit_2_e();
	void bit_2_h();
	void bit_2_l();
	void bit_3_a();
	void bit_3_b();
	void bit_3_c();
	void bit_3_d();
	void bit_3_e();
	void bit_3_h();
	void bit_3_l();
	void bit_4_a();
	void bit_4_b();
	void bit_4_c();
	void bit_4_d();
	void bit_4_e();
	void bit_4_h();
	void bit_4_l();
	void bit_5_a();
	void bit_5_b();
	void bit_5_c();
	void bit_5_d();
	void bit_5_e();
	void bit_5_h();
	void bit_5_l();
	void bit_6_a();
	void bit_6_b();
	void bit_6_c();
	void bit_6_d();
	void bit_6_e();
	void bit_6_h();
	void bit_6_l();
	void bit_7_a();
	void bit_7_b();
	void bit_7_c();
	void bit_7_d();
	void bit_7_e();
	void bit_7_h();
	void bit_7_l();

	inline void BIT_HL(byte bit);
	void bit_0_xhl();
	void bit_1_xhl();
	void bit_2_xhl();
	void bit_3_xhl();
	void bit_4_xhl();
	void bit_5_xhl();
	void bit_6_xhl();
	void bit_7_xhl();

	inline void BIT_IX(byte bit);
	void bit_0_xix();
	void bit_1_xix();
	void bit_2_xix();
	void bit_3_xix();
	void bit_4_xix();
	void bit_5_xix();
	void bit_6_xix();
	void bit_7_xix();

	inline void BIT_IY(byte bit);
	void bit_0_xiy();
	void bit_1_xiy();
	void bit_2_xiy();
	void bit_3_xiy();
	void bit_4_xiy();
	void bit_5_xiy();
	void bit_6_xiy();
	void bit_7_xiy();

	inline byte RES(byte bit, byte reg);
	void res_0_a();
	void res_0_b();
	void res_0_c();
	void res_0_d();
	void res_0_e();
	void res_0_h();
	void res_0_l();
	void res_1_a();
	void res_1_b();
	void res_1_c();
	void res_1_d();
	void res_1_e();
	void res_1_h();
	void res_1_l();
	void res_2_a();
	void res_2_b();
	void res_2_c();
	void res_2_d();
	void res_2_e();
	void res_2_h();
	void res_2_l();
	void res_3_a();
	void res_3_b();
	void res_3_c();
	void res_3_d();
	void res_3_e();
	void res_3_h();
	void res_3_l();
	void res_4_a();
	void res_4_b();
	void res_4_c();
	void res_4_d();
	void res_4_e();
	void res_4_h();
	void res_4_l();
	void res_5_a();
	void res_5_b();
	void res_5_c();
	void res_5_d();
	void res_5_e();
	void res_5_h();
	void res_5_l();
	void res_6_a();
	void res_6_b();
	void res_6_c();
	void res_6_d();
	void res_6_e();
	void res_6_h();
	void res_6_l();
	void res_7_a();
	void res_7_b();
	void res_7_c();
	void res_7_d();
	void res_7_e();
	void res_7_h();
	void res_7_l();

	inline byte RES_X(byte bit, word x);
	inline byte RES_X_(byte bit, word x);
	inline byte RES_X_X(byte bit);
	inline byte RES_X_Y(byte bit);
	void res_0_xhl();
	void res_1_xhl();
	void res_2_xhl();
	void res_3_xhl();
	void res_4_xhl();
	void res_5_xhl();
	void res_6_xhl();
	void res_7_xhl();
	void res_0_xix();
	void res_1_xix();
	void res_2_xix();
	void res_3_xix();
	void res_4_xix();
	void res_5_xix();
	void res_6_xix();
	void res_7_xix();
	void res_0_xiy();
	void res_1_xiy();
	void res_2_xiy();
	void res_3_xiy();
	void res_4_xiy();
	void res_5_xiy();
	void res_6_xiy();
	void res_7_xiy();
	void res_0_xix_a();
	void res_0_xix_b();
	void res_0_xix_c();
	void res_0_xix_d();
	void res_0_xix_e();
	void res_0_xix_h();
	void res_0_xix_l();
	void res_1_xix_a();
	void res_1_xix_b();
	void res_1_xix_c();
	void res_1_xix_d();
	void res_1_xix_e();
	void res_1_xix_h();
	void res_1_xix_l();
	void res_2_xix_a();
	void res_2_xix_b();
	void res_2_xix_c();
	void res_2_xix_d();
	void res_2_xix_e();
	void res_2_xix_h();
	void res_2_xix_l();
	void res_3_xix_a();
	void res_3_xix_b();
	void res_3_xix_c();
	void res_3_xix_d();
	void res_3_xix_e();
	void res_3_xix_h();
	void res_3_xix_l();
	void res_4_xix_a();
	void res_4_xix_b();
	void res_4_xix_c();
	void res_4_xix_d();
	void res_4_xix_e();
	void res_4_xix_h();
	void res_4_xix_l();
	void res_5_xix_a();
	void res_5_xix_b();
	void res_5_xix_c();
	void res_5_xix_d();
	void res_5_xix_e();
	void res_5_xix_h();
	void res_5_xix_l();
	void res_6_xix_a();
	void res_6_xix_b();
	void res_6_xix_c();
	void res_6_xix_d();
	void res_6_xix_e();
	void res_6_xix_h();
	void res_6_xix_l();
	void res_7_xix_a();
	void res_7_xix_b();
	void res_7_xix_c();
	void res_7_xix_d();
	void res_7_xix_e();
	void res_7_xix_h();
	void res_7_xix_l();
	void res_0_xiy_a();
	void res_0_xiy_b();
	void res_0_xiy_c();
	void res_0_xiy_d();
	void res_0_xiy_e();
	void res_0_xiy_h();
	void res_0_xiy_l();
	void res_1_xiy_a();
	void res_1_xiy_b();
	void res_1_xiy_c();
	void res_1_xiy_d();
	void res_1_xiy_e();
	void res_1_xiy_h();
	void res_1_xiy_l();
	void res_2_xiy_a();
	void res_2_xiy_b();
	void res_2_xiy_c();
	void res_2_xiy_d();
	void res_2_xiy_e();
	void res_2_xiy_h();
	void res_2_xiy_l();
	void res_3_xiy_a();
	void res_3_xiy_b();
	void res_3_xiy_c();
	void res_3_xiy_d();
	void res_3_xiy_e();
	void res_3_xiy_h();
	void res_3_xiy_l();
	void res_4_xiy_a();
	void res_4_xiy_b();
	void res_4_xiy_c();
	void res_4_xiy_d();
	void res_4_xiy_e();
	void res_4_xiy_h();
	void res_4_xiy_l();
	void res_5_xiy_a();
	void res_5_xiy_b();
	void res_5_xiy_c();
	void res_5_xiy_d();
	void res_5_xiy_e();
	void res_5_xiy_h();
	void res_5_xiy_l();
	void res_6_xiy_a();
	void res_6_xiy_b();
	void res_6_xiy_c();
	void res_6_xiy_d();
	void res_6_xiy_e();
	void res_6_xiy_h();
	void res_6_xiy_l();
	void res_7_xiy_a();
	void res_7_xiy_b();
	void res_7_xiy_c();
	void res_7_xiy_d();
	void res_7_xiy_e();
	void res_7_xiy_h();
	void res_7_xiy_l();

	inline byte SET(byte bit, byte reg);
	void set_0_a();
	void set_0_b();
	void set_0_c();
	void set_0_d();
	void set_0_e();
	void set_0_h();
	void set_0_l();
	void set_1_a();
	void set_1_b();
	void set_1_c();
	void set_1_d();
	void set_1_e();
	void set_1_h();
	void set_1_l();
	void set_2_a();
	void set_2_b();
	void set_2_c();
	void set_2_d();
	void set_2_e();
	void set_2_h();
	void set_2_l();
	void set_3_a();
	void set_3_b();
	void set_3_c();
	void set_3_d();
	void set_3_e();
	void set_3_h();
	void set_3_l();
	void set_4_a();
	void set_4_b();
	void set_4_c();
	void set_4_d();
	void set_4_e();
	void set_4_h();
	void set_4_l();
	void set_5_a();
	void set_5_b();
	void set_5_c();
	void set_5_d();
	void set_5_e();
	void set_5_h();
	void set_5_l();
	void set_6_a();
	void set_6_b();
	void set_6_c();
	void set_6_d();
	void set_6_e();
	void set_6_h();
	void set_6_l();
	void set_7_a();
	void set_7_b();
	void set_7_c();
	void set_7_d();
	void set_7_e();
	void set_7_h();
	void set_7_l();

	inline byte SET_X(byte bit, word x);
	inline byte SET_X_(byte bit, word x);
	inline byte SET_X_X(byte bit);
	inline byte SET_X_Y(byte bit);
	void set_0_xhl();
	void set_1_xhl();
	void set_2_xhl();
	void set_3_xhl();
	void set_4_xhl();
	void set_5_xhl();
	void set_6_xhl();
	void set_7_xhl();
	void set_0_xix();
	void set_1_xix();
	void set_2_xix();
	void set_3_xix();
	void set_4_xix();
	void set_5_xix();
	void set_6_xix();
	void set_7_xix();
	void set_0_xiy();
	void set_1_xiy();
	void set_2_xiy();
	void set_3_xiy();
	void set_4_xiy();
	void set_5_xiy();
	void set_6_xiy();
	void set_7_xiy();
	void set_0_xix_a();
	void set_0_xix_b();
	void set_0_xix_c();
	void set_0_xix_d();
	void set_0_xix_e();
	void set_0_xix_h();
	void set_0_xix_l();
	void set_1_xix_a();
	void set_1_xix_b();
	void set_1_xix_c();
	void set_1_xix_d();
	void set_1_xix_e();
	void set_1_xix_h();
	void set_1_xix_l();
	void set_2_xix_a();
	void set_2_xix_b();
	void set_2_xix_c();
	void set_2_xix_d();
	void set_2_xix_e();
	void set_2_xix_h();
	void set_2_xix_l();
	void set_3_xix_a();
	void set_3_xix_b();
	void set_3_xix_c();
	void set_3_xix_d();
	void set_3_xix_e();
	void set_3_xix_h();
	void set_3_xix_l();
	void set_4_xix_a();
	void set_4_xix_b();
	void set_4_xix_c();
	void set_4_xix_d();
	void set_4_xix_e();
	void set_4_xix_h();
	void set_4_xix_l();
	void set_5_xix_a();
	void set_5_xix_b();
	void set_5_xix_c();
	void set_5_xix_d();
	void set_5_xix_e();
	void set_5_xix_h();
	void set_5_xix_l();
	void set_6_xix_a();
	void set_6_xix_b();
	void set_6_xix_c();
	void set_6_xix_d();
	void set_6_xix_e();
	void set_6_xix_h();
	void set_6_xix_l();
	void set_7_xix_a();
	void set_7_xix_b();
	void set_7_xix_c();
	void set_7_xix_d();
	void set_7_xix_e();
	void set_7_xix_h();
	void set_7_xix_l();
	void set_0_xiy_a();
	void set_0_xiy_b();
	void set_0_xiy_c();
	void set_0_xiy_d();
	void set_0_xiy_e();
	void set_0_xiy_h();
	void set_0_xiy_l();
	void set_1_xiy_a();
	void set_1_xiy_b();
	void set_1_xiy_c();
	void set_1_xiy_d();
	void set_1_xiy_e();
	void set_1_xiy_h();
	void set_1_xiy_l();
	void set_2_xiy_a();
	void set_2_xiy_b();
	void set_2_xiy_c();
	void set_2_xiy_d();
	void set_2_xiy_e();
	void set_2_xiy_h();
	void set_2_xiy_l();
	void set_3_xiy_a();
	void set_3_xiy_b();
	void set_3_xiy_c();
	void set_3_xiy_d();
	void set_3_xiy_e();
	void set_3_xiy_h();
	void set_3_xiy_l();
	void set_4_xiy_a();
	void set_4_xiy_b();
	void set_4_xiy_c();
	void set_4_xiy_d();
	void set_4_xiy_e();
	void set_4_xiy_h();
	void set_4_xiy_l();
	void set_5_xiy_a();
	void set_5_xiy_b();
	void set_5_xiy_c();
	void set_5_xiy_d();
	void set_5_xiy_e();
	void set_5_xiy_h();
	void set_5_xiy_l();
	void set_6_xiy_a();
	void set_6_xiy_b();
	void set_6_xiy_c();
	void set_6_xiy_d();
	void set_6_xiy_e();
	void set_6_xiy_h();
	void set_6_xiy_l();
	void set_7_xiy_a();
	void set_7_xiy_b();
	void set_7_xiy_c();
	void set_7_xiy_d();
	void set_7_xiy_e();
	void set_7_xiy_h();
	void set_7_xiy_l();

	inline byte RL(byte reg);
	inline byte RL_X(word x);
	inline byte RL_X_(word x);
	inline byte RL_X_X();
	inline byte RL_X_Y();
	void rl_a();
	void rl_b();
	void rl_c();
	void rl_d();
	void rl_e();
	void rl_h();
	void rl_l();
	void rl_xhl();
	void rl_xix  ();
	void rl_xix_a();
	void rl_xix_b();
	void rl_xix_c();
	void rl_xix_d();
	void rl_xix_e();
	void rl_xix_h();
	void rl_xix_l();
	void rl_xiy  ();
	void rl_xiy_a();
	void rl_xiy_b();
	void rl_xiy_c();
	void rl_xiy_d();
	void rl_xiy_e();
	void rl_xiy_h();
	void rl_xiy_l();

	inline byte RLC(byte reg);
	inline byte RLC_X(word x);
	inline byte RLC_X_(word x);
	inline byte RLC_X_X();
	inline byte RLC_X_Y();
	void rlc_a();
	void rlc_b();
	void rlc_c();
	void rlc_d();
	void rlc_e();
	void rlc_h();
	void rlc_l();
	void rlc_xhl();
	void rlc_xix  ();
	void rlc_xix_a();
	void rlc_xix_b();
	void rlc_xix_c();
	void rlc_xix_d();
	void rlc_xix_e();
	void rlc_xix_h();
	void rlc_xix_l();
	void rlc_xiy  ();
	void rlc_xiy_a();
	void rlc_xiy_b();
	void rlc_xiy_c();
	void rlc_xiy_d();
	void rlc_xiy_e();
	void rlc_xiy_h();
	void rlc_xiy_l();

	inline byte RR(byte reg);
	inline byte RR_X(word x);
	inline byte RR_X_(word x);
	inline byte RR_X_X();
	inline byte RR_X_Y();
	void rr_a();
	void rr_b();
	void rr_c();
	void rr_d();
	void rr_e();
	void rr_h();
	void rr_l();
	void rr_xhl();
	void rr_xix  ();
	void rr_xix_a();
	void rr_xix_b();
	void rr_xix_c();
	void rr_xix_d();
	void rr_xix_e();
	void rr_xix_h();
	void rr_xix_l();
	void rr_xiy  ();
	void rr_xiy_a();
	void rr_xiy_b();
	void rr_xiy_c();
	void rr_xiy_d();
	void rr_xiy_e();
	void rr_xiy_h();
	void rr_xiy_l();

	inline byte RRC(byte reg);
	inline byte RRC_X(word x);
	inline byte RRC_X_(word x);
	inline byte RRC_X_X();
	inline byte RRC_X_Y();
	void rrc_a();
	void rrc_b();
	void rrc_c();
	void rrc_d();
	void rrc_e();
	void rrc_h();
	void rrc_l();
	void rrc_xhl();
	void rrc_xix  ();
	void rrc_xix_a();
	void rrc_xix_b();
	void rrc_xix_c();
	void rrc_xix_d();
	void rrc_xix_e();
	void rrc_xix_h();
	void rrc_xix_l();
	void rrc_xiy  ();
	void rrc_xiy_a();
	void rrc_xiy_b();
	void rrc_xiy_c();
	void rrc_xiy_d();
	void rrc_xiy_e();
	void rrc_xiy_h();
	void rrc_xiy_l();

	inline byte SLA(byte reg);
	inline byte SLA_X(word x);
	inline byte SLA_X_(word x);
	inline byte SLA_X_X();
	inline byte SLA_X_Y();
	void sla_a();
	void sla_b();
	void sla_c();
	void sla_d();
	void sla_e();
	void sla_h();
	void sla_l();
	void sla_xhl();
	void sla_xix  ();
	void sla_xix_a();
	void sla_xix_b();
	void sla_xix_c();
	void sla_xix_d();
	void sla_xix_e();
	void sla_xix_h();
	void sla_xix_l();
	void sla_xiy  ();
	void sla_xiy_a();
	void sla_xiy_b();
	void sla_xiy_c();
	void sla_xiy_d();
	void sla_xiy_e();
	void sla_xiy_h();
	void sla_xiy_l();

	inline byte SLL(byte reg);
	inline byte SLL_X(word x);
	inline byte SLL_X_(word x);
	inline byte SLL_X_X();
	inline byte SLL_X_Y();
	void sll_a();
	void sll_b();
	void sll_c();
	void sll_d();
	void sll_e();
	void sll_h();
	void sll_l();
	void sll_xhl();
	void sll_xix  ();
	void sll_xix_a();
	void sll_xix_b();
	void sll_xix_c();
	void sll_xix_d();
	void sll_xix_e();
	void sll_xix_h();
	void sll_xix_l();
	void sll_xiy  ();
	void sll_xiy_a();
	void sll_xiy_b();
	void sll_xiy_c();
	void sll_xiy_d();
	void sll_xiy_e();
	void sll_xiy_h();
	void sll_xiy_l();

	inline byte SRA(byte reg);
	inline byte SRA_X(word x);
	inline byte SRA_X_(word x);
	inline byte SRA_X_X();
	inline byte SRA_X_Y();
	void sra_a();
	void sra_b();
	void sra_c();
	void sra_d();
	void sra_e();
	void sra_h();
	void sra_l();
	void sra_xhl();
	void sra_xix  ();
	void sra_xix_a();
	void sra_xix_b();
	void sra_xix_c();
	void sra_xix_d();
	void sra_xix_e();
	void sra_xix_h();
	void sra_xix_l();
	void sra_xiy  ();
	void sra_xiy_a();
	void sra_xiy_b();
	void sra_xiy_c();
	void sra_xiy_d();
	void sra_xiy_e();
	void sra_xiy_h();
	void sra_xiy_l();

	inline byte SRL(byte reg);
	inline byte SRL_X(word x);
	inline byte SRL_X_(word x);
	inline byte SRL_X_X();
	inline byte SRL_X_Y();
	void srl_a();
	void srl_b();
	void srl_c();
	void srl_d();
	void srl_e();
	void srl_h();
	void srl_l();
	void srl_xhl();
	void srl_xix  ();
	void srl_xix_a();
	void srl_xix_b();
	void srl_xix_c();
	void srl_xix_d();
	void srl_xix_e();
	void srl_xix_h();
	void srl_xix_l();
	void srl_xiy  ();
	void srl_xiy_a();
	void srl_xiy_b();
	void srl_xiy_c();
	void srl_xiy_d();
	void srl_xiy_e();
	void srl_xiy_h();
	void srl_xiy_l();

	void rla();
	void rlca();
	void rra();
	void rrca();

	void rld();
	void rrd();

	inline void PUSH(word reg);
	void push_af();
	void push_bc();
	void push_de();
	void push_hl();
	void push_ix();
	void push_iy();

	inline word POP();
	void pop_af();
	void pop_bc();
	void pop_de();
	void pop_hl();
	void pop_ix();
	void pop_iy();

	inline void CALL();
	inline void SKIP_JP();
	inline void SKIP_JR();
	void call();
	void call_c();
	void call_m();
	void call_nc();
	void call_nz();
	void call_p();
	void call_pe();
	void call_po();
	void call_z();

	inline void RST(word x);
	void rst_00();
	void rst_08();
	void rst_10();
	void rst_18();
	void rst_20();
	void rst_28();
	void rst_30();
	void rst_38();

	inline void RET();
	void ret();
	void ret_c();
	void ret_m();
	void ret_nc();
	void ret_nz();
	void ret_p();
	void ret_pe();
	void ret_po();
	void ret_z();
	void reti();
	void retn();

	void jp_hl();
	void jp_ix();
	void jp_iy();

	inline void JP();
	void jp();
	void jp_c();
	void jp_m();
	void jp_nc();
	void jp_nz();
	void jp_p();
	void jp_pe();
	void jp_po();
	void jp_z();

	inline void JR();
	void jr();
	void jr_c();
	void jr_nc();
	void jr_nz();
	void jr_z();
	void djnz();

	inline word EX_SP(word reg);
	void ex_xsp_hl();
	void ex_xsp_ix();
	void ex_xsp_iy();

	inline byte IN();
	void in_a_c();
	void in_b_c();
	void in_c_c();
	void in_d_c();
	void in_e_c();
	void in_h_c();
	void in_l_c();
	void in_0_c();

	void in_a_byte();

	inline void OUT(byte val);
	void out_c_a();
	void out_c_b();
	void out_c_c();
	void out_c_d();
	void out_c_e();
	void out_c_h();
	void out_c_l();
	void out_c_0();

	void out_byte_a();

	inline void BLOCK_CP(bool increase, bool repeat);
	void cpd();
	void cpi();
	void cpdr();
	void cpir();

	inline void BLOCK_LD(bool increase, bool repeat);
	void ldd();
	void ldi();
	void lddr();
	void ldir();

	inline void BLOCK_IN(bool increase, bool repeat);
	void ind();
	void ini();
	void indr();
	void inir();

	inline void BLOCK_OUT(bool increase, bool repeat);
	void outd();
	void outi();
	void otdr();
	void otir();

	void nop();
	void ccf();
	void cpl();
	void daa();
	void neg();
	void scf();
	void ex_af_af();
	void ex_de_hl();
	void exx();
	void di();
	void ei();
	void halt();
	void im_0();
	void im_1();
	void im_2();

	void ld_a_i();
	void ld_a_r();
	void ld_i_a();
	void ld_r_a();

	inline void MULUB(byte reg);
	void mulub_a_xhl();
	void mulub_a_a();
	void mulub_a_b();
	void mulub_a_c();
	void mulub_a_d();
	void mulub_a_e();
	void mulub_a_h();
	void mulub_a_l();

	inline void MULUW(word reg);
	void muluw_hl_bc();
	void muluw_hl_de();
	void muluw_hl_hl();
	void muluw_hl_sp();

	void dd_cb();
	void fd_cb();
	void cb();
	void ed();
	void dd();
	void fd();

	friend class MSXCPU;
	friend class Z80TYPE;
	friend class R800TYPE;
};

} // namespace openmsx

#endif
