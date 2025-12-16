/*
 * tests.c - Test suite for RISC-V emulator
 * Run with: make test
 */

#include "emu.h"

#include <assert.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────────────
 * Helper: reset state
 * ───────────────────────────────────────────────────────────────────────────── */
static void reset_state(risc_v_state *state) {
  memset(state, 0, sizeof(*state));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Test loader functions
 * ───────────────────────────────────────────────────────────────────────────── */

// ADD: x3 = x1 + x2   (5 + 7 = 12)
void load_test_add_program(risc_v_state *state) {
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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
  if (!state) return;
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

// SW: store word - sw x2, 0(x1)
void load_test_sw_program(risc_v_state *state) {
  if (!state) return;
  reset_state(state);
  // Encoding for: sw x2, 0(x1)
  // imm[11:5]=0, rs2=2, rs1=1, funct3=0x2, imm[4:0]=0, opcode=0x23 ->
  // 0x0020A023
  const uint32_t sw_x2_0_x1 = 0x0020A023u;
  state->memory[0] = sw_x2_0_x1;
  // Prepare registers: base in x1, value in x2
  state->regs[REG_x1] = 0;           // base address 0
  state->regs[REG_x2] = 0xDEADBEEFu; // value to store
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// LW: load word - lw x3, 4(x1)
void load_test_lw_program(risc_v_state *state) {
  if (!state) return;
  reset_state(state);
  // Place a data word at memory[1] (byte addr 4)
  state->memory[1] = 0xCAFEBABEu;

  // Encoding for: lw x3, 4(x1)
  // imm=4, rs1=1, funct3=0x2, rd=3, opcode=0x03 -> construct 32-bit
  const uint32_t lw_x3_4_x1 =
      (4u << 20) | (1u << 15) | (0x2u << 12) | (3u << 7) | (0x03u);
  state->memory[0] = lw_x3_4_x1;
  state->regs[REG_x1] = 0; // base
  state->regs[REG_x3] = 0;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

// ADDI: x3 = x1 + 10   (5 + 10 = 15)
void load_test_addi_program(risc_v_state *state) {
  if (!state) return;
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

// JALR: jalr x3, 12(x0) then addi x4, x0, 42 at target
void load_test_jalr_program(risc_v_state *state) {
  if (!state) return;
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

/* ─────────────────────────────────────────────────────────────────────────────
 * Test runner infrastructure
 * ───────────────────────────────────────────────────────────────────────────── */

typedef struct {
  const char *name;
  void (*loader)(risc_v_state *);
  uint32_t expected_x3;
} test_case_t;

static test_case_t tests[] = {
    {"ADD  x3, x1, x2", load_test_add_program, 12},
    {"SUB  x3, x1, x2", load_test_sub_program, 7},
    {"JALR  x3, 12(x0)", load_test_jalr_program, 42},
    {"XOR  x3, x1, x2", load_test_xor_program, 6},
    {"OR   x3, x1, x2", load_test_or_program, 14},
    {"AND  x3, x1, x2", load_test_and_program, 8},
    {"SLL  x3, x1, x2", load_test_sll_program, 6},
    {"SRL  x3, x1, x2", load_test_srl_program, 4},
    {"SRA  x3, x1, x2", load_test_sra_program, 0xFFFFFFFCu},
    {"SLT  x3, x1, x2", load_test_slt_program, 1},
    {"SLTU x3, x1, x2", load_test_sltu_program, 1},
};

#define NUM_TESTS (sizeof(tests) / sizeof(tests[0]))

/* ─────────────────────────────────────────────────────────────────────────────
 * main: run all tests
 * ───────────────────────────────────────────────────────────────────────────── */
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
      printf("[FAIL] %s → x3 = 0x%08X (expected 0x%08X)\n",
             tests[i].name, got, tests[i].expected_x3);
      ++failed;
    }
  }

  free(state);
  printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);
  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
