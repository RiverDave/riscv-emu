/*
 * emu.h - RISC-V Emulator Core Definitions
 */

#ifndef EMU_H
#define EMU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ─────────────────────────────────────────────────────────────────────────────
 * Constants
 * ───────────────────────────────────────────────────────────────────────────── */

#define MEMORY_SIZE 0x00500000      // 20 MB (in 32-bit words)
#define STACK_START_ADDR 0x00400000 

/* ─────────────────────────────────────────────────────────────────────────────
 * Register X-macro: name and printable string
 * ───────────────────────────────────────────────────────────────────────────── */

#define REGISTERS                                                              \
  X(x0, "x0/zero")                                                             \
  X(x1, "x1/ra")                                                               \
  X(x2, "x2/sp")                                                               \
  X(x3, "x3/gp")                                                               \
  X(x4, "x4/tp")                                                               \
  X(x5, "x5/t0")                                                               \
  X(x6, "x6/t1")                                                               \
  X(x7, "x7/t2")                                                               \
  X(x8, "x8/s0/fp")                                                            \
  X(x9, "x9/s1")                                                               \
  X(x10, "x10/a0")                                                             \
  X(x11, "x11/a1")                                                             \
  X(x12, "x12/a2")                                                             \
  X(x13, "x13/a3")                                                             \
  X(x14, "x14/a4")                                                             \
  X(x15, "x15/a5")                                                             \
  X(x16, "x16/a6")                                                             \
  X(x17, "x17/a7")                                                             \
  X(x18, "x18/s2")                                                             \
  X(x19, "x19/s3")                                                             \
  X(x20, "x20/s4")                                                             \
  X(x21, "x21/s5")                                                             \
  X(x22, "x22/s6")                                                             \
  X(x23, "x23/s7")                                                             \
  X(x24, "x24/s8")                                                             \
  X(x25, "x25/s9")                                                             \
  X(x26, "x26/s10")                                                            \
  X(x27, "x27/s11")                                                            \
  X(x28, "x28/t3")                                                             \
  X(x29, "x29/t4")                                                             \
  X(x30, "x30/t5")                                                             \
  X(x31, "x31/t6")

typedef enum {
#define X(name, str) REG_##name,
  REGISTERS
#undef X
      REG_COUNT
} regid_t;

/* ─────────────────────────────────────────────────────────────────────────────
 * Macros
 * ───────────────────────────────────────────────────────────────────────────── */

#define TRY_OR_EXIT(_func, _msg)                                               \
  do {                                                                         \
    if (!(_func)) {                                                            \
      fprintf(stderr, "Error in %s at line %d: %s\n", __FILE__, __LINE__,      \
              _msg);                                                           \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

/* ─────────────────────────────────────────────────────────────────────────────
 * Core state
 * ───────────────────────────────────────────────────────────────────────────── */

typedef struct {
  uint32_t regs[REG_COUNT];
  uint32_t memory[MEMORY_SIZE];
  uint32_t pc;
  bool is_valid;
  bool is_running;
} risc_v_state;

/* ─────────────────────────────────────────────────────────────────────────────
 * Opcodes / Instruction formats
 * ───────────────────────────────────────────────────────────────────────────── */

/* ─────────────────────────────────────────────────────────────────────────────
 * Opcode X-macro: name only
 * ───────────────────────────────────────────────────────────────────────────── */

#define OPCODES \
  X(INVALID) \
  X(ADD) \
  X(SUB) \
  X(ADDI) \
  X(XORI) \
  X(ORI) \
  X(ANDI) \
  X(SLLI) \
  X(SRLI) \
  X(SRAI) \
  X(SLTI) \
  X(SLTIU) \
  X(SW) \
  X(LW) \
  X(LB) \
  X(LH) \
  X(LBU) \
  X(LHU) \
  X(JALR) \
  X(XOR) \
  X(OR) \
  X(AND) \
  X(SLL) \
  X(SRL) \
  X(SRA) \
  X(SLT) \
  X(SLTU)

typedef enum {
#define X(name) name,
  OPCODES
#undef X
  OPCODE_COUNT
} Opcode;

// source:
// https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf
typedef enum {
  R_FMT = 0x33,
  I_FMT = 0x13,
  LOAD_FMT = 0x03,
  S_FMT = 0x23,
  I_JMP_FMT = 0x67, // jalr
  SB_FMT,           // TODO
  UJ_FMT
} IFormat;

typedef struct {
  Opcode name;
  uint32_t ops[3];
} Instruction;

/* ─────────────────────────────────────────────────────────────────────────────
 * Function declarations
 * ───────────────────────────────────────────────────────────────────────────── */

bool init_riscv_emu(risc_v_state **state);
bool is_address_valid(uint32_t addr);
bool decode_instruction(risc_v_state *state, uint32_t instruction,
                        Instruction *output);
void execute_instruction(risc_v_state *state, const Instruction *insn);
bool is_riscv_state_valid(const risc_v_state *state);
bool emulate(risc_v_state *state);
void print_registers(const risc_v_state *state);
void print_memory(const risc_v_state *state, uint32_t start_addr_bytes,
                  uint32_t word_count);
const char *opcode_to_string(Opcode op);

#endif /* EMU_H */
