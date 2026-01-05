/*
 * tests.c - Test suite for RISC-V emulator
 * Run with: make test
 */

#include "emu.h"

#include <assert.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────────────
 * Helper: reset state
 * ─────────────────────────────────────────────────────────────────────────────
 */
static void reset_state(risc_v_state *state) {
  memset(state, 0, sizeof(*state));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Test loader functions
 * ─────────────────────────────────────────────────────────────────────────────
 */

// ADD: x3 = x1 + x2   (5 + 7 = 12)
void load_test_add_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: add x3, x1, x2  -> 0x002081B3
  const uint32_t add_x3_x1_x2 = 0x002081B3u;
  state->memory[0] = add_x3_x1_x2;
  state->regs[REG_x1] = 5;
  state->regs[REG_x2] = 7;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SUB: x3 = x1 - x2   (10 - 3 = 7)
void load_test_sub_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: sub x3, x1, x2  -> 0x402081B3
  const uint32_t sub_x3_x1_x2 = 0x402081B3u;
  state->memory[0] = sub_x3_x1_x2;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 3;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// XOR: x3 = x1 ^ x2   (0b1010 ^ 0b1100 = 0b0110 = 6)
void load_test_xor_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: xor x3, x1, x2  -> 0x0020C1B3
  const uint32_t xor_x3_x1_x2 = 0x0020C1B3u;
  state->memory[0] = xor_x3_x1_x2;
  state->regs[REG_x1] = 10; // 0b1010
  state->regs[REG_x2] = 12; // 0b1100
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// OR: x3 = x1 | x2   (0b1010 | 0b1100 = 0b1110 = 14)
void load_test_or_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);

  // Encoding for: or x3, x1, x2  -> 0x0020E1B3
  // 0000000(f7)|00010(r2)|00001(rs1)|110(f3)|00011(rd)|0110011(op)
  // 0000 0000 0010 0000 1110 0001 1011 0011
  // 0020E1B3

  const uint32_t or_x3_x1_x2 = 0x0020E1B3u;
  state->memory[0] = or_x3_x1_x2;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 12;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// AND: x3 = x1 & x2   (0b1010 & 0b1100 = 0b1000 = 8)
void load_test_and_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: and x3, x1, x2  -> 0x0020F1B3
  const uint32_t and_x3_x1_x2 = 0x0020F1B3u;
  state->memory[0] = and_x3_x1_x2;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 12;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SLL: x3 = x1 << x2   (3 << 1 = 6)
void load_test_sll_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: sll x3, x1, x2  -> 0x002091B3
  const uint32_t sll_x3_x1_x2 = 0x002091B3u;
  state->memory[0] = sll_x3_x1_x2;
  state->regs[REG_x1] = 3;
  state->regs[REG_x2] = 1;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SRL: x3 = x1 >> x2 (logical)   (8 >> 1 = 4)
void load_test_srl_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: srl x3, x1, x2  -> 0x0020D1B3
  const uint32_t srl_x3_x1_x2 = 0x0020D1B3u;
  state->memory[0] = srl_x3_x1_x2;
  state->regs[REG_x1] = 8;
  state->regs[REG_x2] = 1;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SRA: x3 = x1 >> x2 (arithmetic)   (-8 >> 1 = -4 = 0xFFFFFFFC)
void load_test_sra_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: sra x3, x1, x2  -> 0x4020D1B3
  const uint32_t sra_x3_x1_x2 = 0x4020D1B3u;
  state->memory[0] = sra_x3_x1_x2;
  state->regs[REG_x1] = 0xFFFFFFF8u; // -8 in two's complement
  state->regs[REG_x2] = 1;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SLT: x3 = (x1 < x2) ? 1 : 0 (signed)   (-1 < 1 → 1)
void load_test_slt_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: slt x3, x1, x2  -> 0x0020A1B3
  const uint32_t slt_x3_x1_x2 = 0x0020A1B3u;
  state->memory[0] = slt_x3_x1_x2;
  state->regs[REG_x1] = 0xFFFFFFFFu; // -1 signed
  state->regs[REG_x2] = 1;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SLTU: x3 = (x1 < x2) ? 1 : 0 (unsigned)   (1 < 2 → 1)
void load_test_sltu_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: sltu x3, x1, x2  -> 0x0020B1B3
  const uint32_t sltu_x3_x1_x2 = 0x0020B1B3u;
  state->memory[0] = sltu_x3_x1_x2;
  state->regs[REG_x1] = 1u;
  state->regs[REG_x2] = 2u;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SB: Store byte - sb x5, 0(x1)
// Store byte 0xAB to memory[1] at byte offset 0
void load_test_sb_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  // Encoding for: sb x5, 0(x1)
  // imm[11:5]=0, rs2=5, rs1=1, funct3=0x0, imm[4:0]=0, opcode=0x23
  const uint32_t sb_x5_0_x1 =
      (0u << 25) | (5u << 20) | (1u << 15) | (0x0u << 12) | (0u << 7) | (0x23u);
  state->memory[0] = sb_x5_0_x1;

  // Data will be stored at memory[1]
  state->memory[1] = 0x00000000u; // Initially cleared

  state->regs[REG_x1] = 4;     // base address (points to memory[1])
  state->regs[REG_x5] = 0xABu; // byte value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SB: Store byte with different value - sb x5, 0(x1)
// Store byte 0xFF to memory[1]
void load_test_sb_0xff_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0]
  const uint32_t sb_x5_0_x1 =
      (0u << 25) | (5u << 20) | (1u << 15) | (0x0u << 12) | (0u << 7) | (0x23u);
  state->memory[0] = sb_x5_0_x1;

  state->memory[1] = 0x00000000u;

  state->regs[REG_x1] = 4;     // base address
  state->regs[REG_x5] = 0xFFu; // byte value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SH: Store halfword - sh x5, 0(x1)
// Store halfword 0x1234 to memory[1] at offset 0
void load_test_sh_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0]
  // Encoding for: sh x5, 0(x1)
  // imm[11:5]=0, rs2=5, rs1=1, funct3=0x1, imm[4:0]=0, opcode=0x23
  const uint32_t sh_x5_0_x1 =
      (0u << 25) | (5u << 20) | (1u << 15) | (0x1u << 12) | (0u << 7) | (0x23u);
  state->memory[0] = sh_x5_0_x1;

  // Data will be stored at memory[1]
  state->memory[1] = 0x00000000u; // Initially cleared

  state->regs[REG_x1] = 4;       // base address
  state->regs[REG_x5] = 0x1234u; // halfword value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SH: Store halfword - sh x5, 0(x1)
// Store halfword 0x8000 (with sign bit) to memory[1]
void load_test_sh_negative_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0]
  const uint32_t sh_x5_0_x1 =
      (0u << 25) | (5u << 20) | (1u << 15) | (0x1u << 12) | (0u << 7) | (0x23u);
  state->memory[0] = sh_x5_0_x1;

  state->memory[1] = 0x00000000u;

  state->regs[REG_x1] = 4;       // base address
  state->regs[REG_x5] = 0x8000u; // halfword value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SW: Store word - sw x5, 0(x1)
// Store word 0xDEADBEEF to memory[1]
void load_test_sw_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0]
  // Encoding for: sw x5, 0(x1)
  // imm[11:5]=0, rs2=5, rs1=1, funct3=0x2, imm[4:0]=0, opcode=0x23
  const uint32_t sw_x5_0_x1 =
      (0u << 25) | (5u << 20) | (1u << 15) | (0x2u << 12) | (0u << 7) | (0x23u);
  state->memory[0] = sw_x5_0_x1;

  // Data will be stored at memory[1]
  state->memory[1] = 0x00000000u; // Initially cleared

  state->regs[REG_x1] = 4;           // base address
  state->regs[REG_x5] = 0xDEADBEEFu; // word value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SW: Store word to different location - sw x5, 0(x1)
// Store word to memory[2]
void load_test_sw_offset_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0]
  const uint32_t sw_x5_0_x1 =
      (0u << 25) | (5u << 20) | (1u << 15) | (0x2u << 12) | (0u << 7) | (0x23u);
  state->memory[0] = sw_x5_0_x1;

  // Data will be stored at memory[2]
  state->memory[2] = 0x00000000u; // Initially cleared

  state->regs[REG_x1] = 8;           // base address (points to memory[2])
  state->regs[REG_x5] = 0xCAFEBABEu; // word value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LW: load word - lw x3, 4(x1)
void load_test_lw_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Place a data word at memory[2] (byte addr 8)
  state->memory[2] = 0xCAFEBABEu;

  // Encoding for: lw x3, 8(x1)
  // imm=4, rs1=1, funct3=0x2, rd=3, opcode=0x03 -> construct 32-bit
  const uint32_t lw_x3_4_x1 =
      (8u << 20) | (1u << 15) | (0x2u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lw_x3_4_x1;
  state->regs[REG_x1] = 0; // base
  state->regs[REG_x3] = 0;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// ADDI: x3 = x1 + 10   (5 + 10 = 15)
void load_test_addi_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: addi x3, x1, 10  -> imm[11:0]=0x00A, rs1=1, funct3=0, rd=3,
  // opcode=0x13 instruction = 0x00A08193
  const uint32_t addi_x3_x1_10 = 0x00A08193u;
  state->memory[0] = addi_x3_x1_10;
  state->regs[REG_x1] = 5; // base value
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BEQ: Branch if Equal - beq x1, x2, 8
// True case: x1 == x2, branch taken, x3 = 100
void load_test_beq_true_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t beq_x1_x2_12 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x0u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = beq_x1_x2_12;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 42;
  state->regs[REG_x2] = 42;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BEQ: False case - x1 != x2, branch not taken, x3 = 0
void load_test_beq_false_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t beq_x1_x2_12 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x0u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = beq_x1_x2_12;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 42;
  state->regs[REG_x2] = 99;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BNE: Branch if Not Equal - bne x1, x2, 8
// True case: x1 != x2, branch taken, x3 = 100
void load_test_bne_true_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bne_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x1u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bne_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 42;
  state->regs[REG_x2] = 99;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BNE: False case - x1 == x2, branch not taken, x3 = 0
void load_test_bne_false_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bne_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x1u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bne_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 42;
  state->regs[REG_x2] = 42;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BLT: Branch if Less Than (signed) - blt x1, x2, 8
// True case: x1 < x2, branch taken, x3 = 100
void load_test_blt_true_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t blt_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x4u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = blt_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BLT: False case - x1 >= x2, branch not taken, x3 = 0
void load_test_blt_false_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t blt_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x4u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = blt_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 50;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BGE: Branch if Greater or Equal (signed) - bge x1, x2, 8
// True case: x1 >= x2, branch taken, x3 = 100
void load_test_bge_true_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bge_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x5u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bge_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 50;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BGE: False case - x1 < x2, branch not taken, x3 = 0
void load_test_bge_false_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bge_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x5u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bge_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BLTU: Branch if Less Than Unsigned - bltu x1, x2, 8
// True case: x1 < x2 (unsigned), branch taken, x3 = 100
void load_test_bltu_true_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bltu_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x6u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bltu_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BLTU: False case - x1 >= x2 (unsigned), branch not taken, x3 = 0
void load_test_bltu_false_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bltu_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x6u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bltu_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 50;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BGEU: Branch if Greater or Equal Unsigned - bgeu x1, x2, 8
// True case: x1 >= x2 (unsigned), branch taken, x3 = 100
void load_test_bgeu_true_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bgeu_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x7u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bgeu_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 50;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// BGEU: False case - x1 < x2 (unsigned), branch not taken, x3 = 0
void load_test_bgeu_false_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  const uint32_t bgeu_x1_x2_8 =
      (0u << 25) | (2u << 20) | (1u << 15) | (0x7u << 12) | (3u << 8) | (0x63u);
  state->memory[0] = bgeu_x1_x2_8;
  const uint32_t addi_x3_x0_0 =
      (0u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[1] = addi_x3_x0_0;
  const uint32_t addi_x3_x0_100 =
      (100u << 20) | (0u << 15) | (0x0u << 12) | (3u << 7) | (0x13u);
  state->memory[3] = addi_x3_x0_100;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 50;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// JALR: jalr x3, 12(x0) then addi x4, x0, 42 at target
void load_test_jalr_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);

  // Encoding for: jalr x3, 12(x0)
  // imm=12, rs1=0, funct3=0, rd=3, opcode=0x67
  const uint32_t jalr_imm_bytes = 12u;
  const uint32_t jalr_x3_12_x0 =
      (jalr_imm_bytes << 20) | (0u << 15) | (0u << 12) | (1u << 7) | (0x67u);

  // Encoding for: addi x3, x0, 42 -> (42<<20)|(0<<15)|(0<<12)|(3<<7)|0x13
  const uint32_t addi_x3_x0_42 =
      (42u << 20) | (0u << 15) | (0u << 12) | (3u << 7) | (0x13u);

  state->memory[0] = jalr_x3_12_x0;
  // place the target instruction at the correct word index derived from the
  // byte immediate
  uint32_t target_index = jalr_imm_bytes / sizeof(uint32_t);
  state->memory[target_index] = addi_x3_x0_42; // should execute after jump

  state->regs[REG_x0] = 0;
  state->regs[REG_x3] = 0;
  state->regs[REG_x4] = 0;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SLTI: x3 = (x1 < 10) ? 1 : 0 (signed)   (5 < 10 → 1)
void load_test_slti_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: slti x3, x1, 10
  // imm=10, rs1=1, funct3=0x2, rd=3, opcode=0x13
  const uint32_t slti_x3_x1_10 =
      (10u << 20) | (1u << 15) | (0x2u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = slti_x3_x1_10;
  state->regs[REG_x1] = 5;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SLTIU: x3 = (x1 < 10) ? 1 : 0 (unsigned)   (5 < 10 → 1)
void load_test_sltiu_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: sltiu x3, x1, 10
  // imm=10, rs1=1, funct3=0x3, rd=3, opcode=0x13
  const uint32_t sltiu_x3_x1_10 =
      (10u << 20) | (1u << 15) | (0x3u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = sltiu_x3_x1_10;
  state->regs[REG_x1] = 5;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// XORI: x3 = x1 ^ 0xFF   (0xAA ^ 0xFF = 0x55)
void load_test_xori_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: xori x3, x1, 0xFF
  // imm=0xFF, rs1=1, funct3=0x4, rd=3, opcode=0x13
  const uint32_t xori_x3_x1_0xFF =
      (0xFFu << 20) | (1u << 15) | (0x4u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = xori_x3_x1_0xFF;
  state->regs[REG_x1] = 0xAA;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// ORI: x3 = x1 | 0x0F   (0xF0 | 0x0F = 0xFF)
void load_test_ori_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: ori x3, x1, 0x0F
  // imm=0x0F, rs1=1, funct3=0x6, rd=3, opcode=0x13
  const uint32_t ori_x3_x1_0x0F =
      (0x0Fu << 20) | (1u << 15) | (0x6u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = ori_x3_x1_0x0F;
  state->regs[REG_x1] = 0xF0;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// ANDI: x3 = x1 & 0x0F   (0xFF & 0x0F = 0x0F)
void load_test_andi_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: andi x3, x1, 0x0F
  // imm=0x0F, rs1=1, funct3=0x7, rd=3, opcode=0x13
  const uint32_t andi_x3_x1_0x0F =
      (0x0Fu << 20) | (1u << 15) | (0x7u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = andi_x3_x1_0x0F;
  state->regs[REG_x1] = 0xFF;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SLLI: x3 = x1 << 2   (8 << 2 = 32)
void load_test_slli_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: slli x3, x1, 2
  // imm[11:5]=0, shamt[4:0]=2, rs1=1, funct3=0x1, rd=3, opcode=0x13
  const uint32_t slli_x3_x1_2 =
      (2u << 20) | (1u << 15) | (0x1u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = slli_x3_x1_2;
  state->regs[REG_x1] = 8;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SRLI: x3 = x1 >> 2 (logical)   (32 >> 2 = 8)
void load_test_srli_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: srli x3, x1, 2
  // imm[11:5]=0, shamt[4:0]=2, rs1=1, funct3=0x5, rd=3, opcode=0x13
  const uint32_t srli_x3_x1_2 =
      (2u << 20) | (1u << 15) | (0x5u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = srli_x3_x1_2;
  state->regs[REG_x1] = 32;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// SRAI: x3 = x1 >> 2 (arithmetic)   (-8 >> 2 = -2 = 0xFFFFFFFE)
void load_test_srai_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Encoding for: srai x3, x1, 2
  // imm[11:5]=0x20 (bit 30 set), shamt[4:0]=2, rs1=1, funct3=0x5, rd=3,
  // opcode=0x13
  const uint32_t srai_x3_x1_2 = (0x20u << 25) | (2u << 20) | (1u << 15) |
                                (0x5u << 12) | (3u << 7) | (0x13u);
  state->memory[0] = srai_x3_x1_2;
  state->regs[REG_x1] = 0xFFFFFFF8u; // -8 in two's complement
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LB: Load byte with sign extension - lb x3, 0(x1)
// Load 0xAB from memory[0], sign extend to 0xFFFFFFAB (-85 signed)
void load_test_lb_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lb_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x0u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lb_x3_0_x1;

  // Data at memory[1] (byte address 4)
  state->memory[1] = 0x000000ABu; // Load byte 0xAB

  state->regs[REG_x1] = 4; // base address (points to memory[1])
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LB: Load negative byte - lb x3, 0(x1)
// Load 0xFF from memory, sign extend to 0xFFFFFFFF (-1 signed)
void load_test_lb_negative_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lb_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x0u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lb_x3_0_x1;

  // Data at memory[1] (byte address 4)
  state->memory[1] = 0x000000FFu; // Load byte 0xFF

  state->regs[REG_x1] = 4; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LH: Load halfword with sign extension - lh x3, 0(x1)
// Load 0x8000 from memory, sign extend to 0xFFFF8000 (-32768 signed)
void load_test_lh_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lh_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x1u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lh_x3_0_x1;

  // Data at memory[2] (byte address 0x8)
  state->memory[2] = 0x12348000u; // Load halfword 0x8000 (lower halfword)

  state->regs[REG_x1] = 8; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LH: Load positive halfword - lh x3, 0(x1)
// Load 0x5678 from memory, sign extend to 0x00005678 (positive)
void load_test_lh_positive_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lh_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x1u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lh_x3_0_x1;

  // Data at memory[1] (byte address 4)
  state->memory[1] = 0x12345678u; // Load halfword 0x5678 (lower halfword)

  state->regs[REG_x1] = 4; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LBU: Load byte unsigned - lbu x3, 0(x1)
// Load 0xAB from memory, zero extend to 0x000000AB
void load_test_lbu_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lbu_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x4u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lbu_x3_0_x1;

  // Data at memory[1] (byte address 4)
  state->memory[1] = 0x000000ABu; // Load byte 0xAB

  state->regs[REG_x1] = 4; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LBU: Load byte unsigned 0xFF - lbu x3, 0(x1)
// Load 0xFF from memory, zero extend to 0x000000FF (255 unsigned)
void load_test_lbu_0xff_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lbu_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x4u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lbu_x3_0_x1;

  // Data at memory[1] (byte address 4)
  state->memory[1] = 0x000000FFu; // Load byte 0xFF

  state->regs[REG_x1] = 4; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LHU: Load halfword unsigned - lhu x3, 0(x1)
// Load 0x8000 from memory, zero extend to 0x00008000
void load_test_lhu_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lhu_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x5u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lhu_x3_0_x1;

  // Data at memory[2] (byte address 8)
  state->memory[2] = 0x12348000u; // Load halfword 0x8000 (lower halfword)

  state->regs[REG_x1] = 8; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LHU: Load halfword unsigned from second word - lhu x3, 0(x1)
// Load 0xFFFF from memory[2], zero extend to 0x0000FFFF
void load_test_lhu_offset_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lhu_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x5u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lhu_x3_0_x1;

  // Data at memory[2] (byte address 8)
  state->memory[2] = 0x1234FFFFu; // Load halfword 0xFFFF (lower halfword)

  state->regs[REG_x1] = 8; // base address (points to memory[2])
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LB: Load byte from second word - lb x3, 0(x1)
// Load 0x56 from memory[2], sign extend to 0x00000056
void load_test_lb_offset_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lb_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x0u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lb_x3_0_x1;

  // Data at memory[2] (byte address 8)
  state->memory[2] = 0x12345678u; // byte 0 is 0x78

  state->regs[REG_x1] = 8; // base address
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LB: Load byte with sign from third word - lb x3, 0(x1)
// Load 0x12 from memory[3], sign extend to 0x00000012
void load_test_lb_offset_3_program(risc_v_state *state) {
  if (!state)
    return;
  reset_state(state);
  // Instruction at memory[0] (byte address 0)
  const uint32_t lb_x3_0_x1 =
      (0u << 20) | (1u << 15) | (0x0u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lb_x3_0_x1;

  // Data at memory[3] (byte address 12)
  state->memory[3] = 0x12345678u; // byte 0 is 0x78

  state->regs[REG_x1] = 12; // base address (points to memory[3])
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Test runner infrastructure
 * ─────────────────────────────────────────────────────────────────────────────
 */

typedef struct {
  const char *name;
  void (*loader)(risc_v_state *);
  uint32_t expected_x3;
} test_case_t;

static test_case_t tests[] = {
    // R-format instructions
    {"ADD  x3, x1, x2", load_test_add_program, 12},
    {"SUB  x3, x1, x2", load_test_sub_program, 7},
    {"XOR  x3, x1, x2", load_test_xor_program, 6},
    {"OR   x3, x1, x2", load_test_or_program, 14},
    {"AND  x3, x1, x2", load_test_and_program, 8},
    {"SLL  x3, x1, x2", load_test_sll_program, 6},
    {"SRL  x3, x1, x2", load_test_srl_program, 4},
    {"SRA  x3, x1, x2", load_test_sra_program, 0xFFFFFFFCu},
    {"SLT  x3, x1, x2", load_test_slt_program, 1},
    {"SLTU x3, x1, x2", load_test_sltu_program, 1},

    // I-format instructions (arithmetic/logic)
    {"ADDI  x3, x1, 10", load_test_addi_program, 15},
    {"XORI  x3, x1, 0xFF", load_test_xori_program, 0x55},
    {"ORI   x3, x1, 0x0F", load_test_ori_program, 0xFF},
    {"ANDI  x3, x1, 0x0F", load_test_andi_program, 0x0F},
    {"SLLI  x3, x1, 2", load_test_slli_program, 32},
    {"SLTI  x3, x1, 10", load_test_slti_program, 1},
    {"SLTIU x3, x1, 10", load_test_sltiu_program, 1},
    {"SRLI  x3, x1, 2", load_test_srli_program, 8},
    {"SRAI  x3, x1, 2", load_test_srai_program, 0xFFFFFFFEu},

    // Load instructions (I-format, opcode 0x03)
    {"LW    x3, 4(x1)", load_test_lw_program, 0xCAFEBABEu},
    {"LB    x3, 0(x1)", load_test_lb_program, 0xFFFFFFABu},
    {"LB    x3, 0(x1) [0xFF]", load_test_lb_negative_program, 0xFFFFFFFFu},
    {"LB    x3, 0(x1) [0x78]", load_test_lb_offset_program, 0x00000078u},
    {"LB    x3, 0(x1) [0x78]", load_test_lb_offset_3_program, 0x00000078u},
    {"LH    x3, 0(x1) [0x8000]", load_test_lh_program, 0xFFFF8000u},
    {"LH    x3, 0(x1) [0x5678]", load_test_lh_positive_program, 0x00005678u},

    {"LBU   x3, 0(x1) [0xAB]", load_test_lbu_program, 0x000000ABu},
    {"LBU   x3, 0(x1) [0xFF]", load_test_lbu_0xff_program, 0x000000FFu},
    {"LHU   x3, 0(x1) [0x8000]", load_test_lhu_program, 0x00008000u},
    {"LHU   x3, 0(x1) [0xFFFF]", load_test_lhu_offset_program, 0x0000FFFFu},

    // Store instructions (S-format, opcode 0x23)
    {"SB    x5, 0(x1) [0xAB]", load_test_sb_program, 0},
    {"SB    x5, 0(x1) [0xFF]", load_test_sb_0xff_program, 0},
    {"SH    x5, 0(x1) [0x1234]", load_test_sh_program, 0},
    {"SW    x5, 0(x1) [0xDEADBEEF]", load_test_sw_program, 0},
    {"SW    x5, 0(x1) [0xCAFEBABE]", load_test_sw_offset_program, 0},

    {"JALR  x3, 12(x0)", load_test_jalr_program, 42},

    // Branch instructions (B-format, opcode 0x63)
    {"BEQ  x1, x2, 8 (taken)", load_test_beq_true_program, 100},
    {"BEQ  x1, x2, 8 (not taken)", load_test_beq_false_program, 0},
    {"BNE  x1, x2, 8 (taken)", load_test_bne_true_program, 100},
    {"BNE  x1, x2, 8 (not taken)", load_test_bne_false_program, 0},
    {"BLT  x1, x2, 8 (taken)", load_test_blt_true_program, 100},
    {"BLT  x1, x2, 8 (not taken)", load_test_blt_false_program, 0},
    {"BGE  x1, x2, 8 (taken)", load_test_bge_true_program, 100},
    {"BGE  x1, x2, 8 (not taken)", load_test_bge_false_program, 0},
    {"BLTU x1, x2, 8 (taken)", load_test_bltu_true_program, 100},
    {"BLTU x1, x2, 8 (not taken)", load_test_bltu_false_program, 0},
    {"BGEU x1, x2, 8 (taken)", load_test_bgeu_true_program, 100},
    {"BGEU x1, x2, 8 (not taken)", load_test_bgeu_false_program, 0},

};

#define NUM_TESTS (sizeof(tests) / sizeof(tests[0]))

/* ─────────────────────────────────────────────────────────────────────────────
 * main: run all tests
 * ─────────────────────────────────────────────────────────────────────────────
 */
int main(void) {
  int passed = 0, failed = 0;

  // Allocate on heap - risc_v_state is too large for stack (~8GB)
  risc_v_state *state = malloc(sizeof(risc_v_state));
  if (!state) {
    fprintf(stderr, "Failed to allocate state\n");
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < NUM_TESTS; ++i) {
    tests[i].loader(state);

    printf("=============== [TEST_START] RUNNING: %s ===============\n",
           tests[i].name);

    if (!emulate(state)) {
      printf("[FAIL] %s — emulation error\n", tests[i].name);
      ++failed;
      continue;
    }

    uint32_t got = state->regs[REG_x3];
    if (got == tests[i].expected_x3) {
      printf("[PASS] %s → x3 = 0x%08X\n", tests[i].name, got);
      ++passed;
    } else {
      printf("[FAIL] %s → x3 = 0x%08X (expected 0x%08X)\n", tests[i].name, got,
             tests[i].expected_x3);
      ++failed;
    }
  }

  free(state);
  printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);
  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
