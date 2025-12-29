#include "emu.h"

#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Sign-extend 12-bit immediate to 32-bit
#define SEXT12(x) ((x) & 0x800 ? (x) | 0xFFFFF000 : (x))

bool init_riscv_emu(risc_v_state **state) {
  *state = malloc(sizeof(risc_v_state));

  if (!*state)
    return false;
  for (uint32_t i = 0; i < REG_COUNT; ++i)
    (*state)->regs[i] = 0;
  (*state)->regs[REG_x2] = STACK_START_ADDR; // set sp
  for (uint32_t i = 0; i < MEMORY_SIZE; ++i)
    (*state)->memory[i] = 0;
  (*state)->pc = 0;
  return true;
}

bool is_address_valid(const uint32_t addr) {
  // for simplicity purposes we're only supported aligned addresses.
  if (addr % sizeof(uint32_t) != 0){
    fprintf(stderr, "Address not divisible by word size\n");
    return false;
  }

  uint32_t index = addr / sizeof(uint32_t);
  return index < MEMORY_SIZE;
}

bool decode_instruction(risc_v_state *state, const uint32_t instruction,
                        Instruction *result) {
  if (!state || !result)
    return false;

  uint32_t opcode = ((instruction >> 0) & 0x7F);
#ifdef DEBUG
  fprintf(stderr, "[DEBUG] decoded: instruction=0x%08x opcode=0x%02x pc=0x%x\n",
          instruction, opcode, state->pc);
#endif

// I-type format bit extraction macro
#define I_DECODE(i) \
  uint32_t rd = ((i) >> 7) & 0x1F, funct3 = ((i) >> 12) & 0x7, \
           rs1 = ((i) >> 15) & 0x1F, imm = ((i) >> 20) & 0xFFF

  switch (opcode) {
  case R_FMT: {

    // 0x7F -> 0111 1111 | mask for (opcode) [6:0] bits
    // 0x1F -> 0001 1111 | mask for (rd) [11:7] bits
    // 0x7 -> 0000 0111 | mask for (funct3) [14:12] bits
    // 0x1F -> 0001 1111 | mask for (rs1) [19:15] bits
    // 0x1F -> 0001 1111 | mask for (rs2) [24:20] bits
    // 0x7F -> 0111 1111 | mask for (funct7) [32:25] bits
    uint32_t rd = ((instruction >> 7) & 0x1F);
    uint32_t funct3 = ((instruction >> 12) & 0x7);
    uint32_t rs1 = ((instruction >> 15) & 0x1F);
    uint32_t rs2 = ((instruction >> 20) & 0x1F);
    uint32_t funct7 = ((instruction >> 25) & 0x7F);

    switch (funct3) {
    case 0x00: {
      if (!funct7)
        result->name = ADD;
      else
        result->name = SUB;
    } break;
    case 0x04:
      result->name = XOR;
      break;
    case 0x6:
      result->name = OR;
      break;
    case 0x7:
      result->name = AND;
      break;
    case 0x1:
      result->name = SLL;
      break;
    case 0x5: {
      if (!funct7) // 0x00
        result->name = SRL;
      else
        result->name = SRA;
    } break;

    case 0x02:
      result->name = SLT;
      break;

    case 0x03:
      result->name = SLTU;
      break;

    default:
      return false;
    }
    assert((rd < REG_COUNT && rs1 < REG_COUNT && rs2 < REG_COUNT) &&
           "Invalid register val");
    result->ops[0] = rd;
    result->ops[1] = rs1;
    result->ops[2] = rs2;
    return true; // success
  } break;

  case I_FMT: {
    I_DECODE(instruction);

    switch (funct3) {

    case 0x00: { // addi
      result->name = ADDI;
    } break;
    default:
      assert(false && "I-FMT instruction not implemented");
    }

    result->ops[0] = rd;
    result->ops[1] = rs1;
    result->ops[2] = imm;
    return true; // success
    assert((rd < REG_COUNT && rs1 < REG_COUNT) && "Invalid register val");

  } break;

  case LOAD_FMT: {
    I_DECODE(instruction);

    switch (funct3) {
    case 0x2: // lw
      result->name = LW;
      break;
    default:
      return false;
    }

    result->ops[0] = rd;
    result->ops[1] = rs1;
    result->ops[2] = imm;
    return true;
  } break;

  case I_JMP_FMT: { // used mainly for jalr, Not to confuse with jal which has
                    // its own opcode.
    I_DECODE(instruction);

    if (funct3 ==
        0x00) // might not be needed but just to make sure we get it right.
      result->name = JALR;
    else
      return false;

    result->ops[0] = rd;
    result->ops[1] = rs1;
    result->ops[2] = imm;
    return true;
  } break;

  case S_FMT: {
    uint32_t funct3 = ((instruction >> 12) & 0x7);
    uint32_t rs1 = ((instruction >> 15) & 0x1F);
    uint32_t rs2 = ((instruction >> 20) & 0x1F);

    // imm's are split in this instruction (god knows why lol)
    // these imms need to be sign extended on execution.
    uint32_t imm_1 = ((instruction >> 7) & 0x1F);
    uint32_t imm_2 = ((instruction >> 25) & 0x7F);
    uint32_t imm_final = (imm_2 << 5) | imm_1;

    switch (funct3) {

    case 0x2: { // sw
      result->name = SW;
    } break;
    default:
      assert(false && "S-FMT instruction not implemented");
    }

    // ops: [rs1, rs2, imm]
    result->ops[0] = rs1;
    result->ops[1] = rs2;
    result->ops[2] = imm_final;

    return true;
  } break;
  default: {
    if (!opcode)
      return false; // temporarily, this should crash
                   // Will make it work after we figure out syscalls
  }
  }
  return false;
#undef I_DECODE
}

static const char *reg_names[REG_COUNT] = {
#define X(name, str) [REG_##name] = str,
    REGISTERS
#undef X
};

void print_registers(const risc_v_state *state) {
  if (!state)
    return;
  puts("-- Registers --");
  puts("Idx Name        Hex       Decimal");
  for (int i = 0; i < REG_COUNT; ++i) {
    printf("%2d  %-12s 0x%08x %10d\n", i, reg_names[i], state->regs[i],
           state->regs[i]);
  }
}

void print_memory(const risc_v_state *state, uint32_t start_addr_bytes,
                  uint32_t word_count) {
  if (!state)
    return;
  puts("-- Memory --");
  puts("Addr       Hex");
  uint32_t start_index = start_addr_bytes / sizeof(uint32_t);
  for (uint32_t i = 0; i < word_count && (start_index + i) < MEMORY_SIZE; ++i) {
    uint32_t index = start_index + i;
    printf("0x%08x 0x%08x\n", (unsigned int)(index * sizeof(uint32_t)),
           state->memory[index]);
  }
}

bool is_riscv_state_valid(const risc_v_state *state) {
  // may add more stuff in here
  if (!state)
    return false;
  if (!state->is_valid || !state->is_running)
    return false;
  // pc is a byte address; convert to word index for memory accesses
  if (state->pc % sizeof(uint32_t) != 0)
    return false;
  uint32_t index = state->pc / sizeof(uint32_t);
  if (index >= MEMORY_SIZE)
    return false;
  return state->memory[index] != 0x00;
}

void execute_instruction(risc_v_state *state, const Instruction *insn) {
  if (!state || !insn)
    return;

// R-format instruction execution macro
#define R_OP(name, op) \
  case name: { \
    uint32_t rd = insn->ops[0], rs1 = insn->ops[1], rs2 = insn->ops[2]; \
    if (rd != REG_x0) state->regs[rd] = state->regs[rs1] op state->regs[rs2]; \
    state->pc += 4; break; }

  switch (insn->name) {
  R_OP(ADD, +)
  R_OP(SUB, -)
  case ADDI: {
    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    int32_t imm = SEXT12(insn->ops[2]);

    if (rd != REG_x0)
      state->regs[rd] = state->regs[rs1] + imm;
    state->pc += 4;
  } break;

  case SW: {

    // Store word: compute effective address and write rs2 value into memory
    uint32_t rs1 = insn->ops[0]; // base register
    uint32_t rs2 = insn->ops[1]; // source register (value to store)
    int32_t imm = SEXT12(insn->ops[2]);

    uint32_t addr = state->regs[rs1] + imm; // byte address


    // FIXME The quality of these errors might not be to good rn. will come back
    // later.
    TRY_OR_EXIT(is_address_valid(addr), "Invalid address for SW Instruction");
    uint32_t index = addr / sizeof(uint32_t);

    state->memory[index] = state->regs[rs2];
    state->pc += 4;
  } break;

  case LW: {

    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    int32_t imm = SEXT12(insn->ops[2]);

    uint32_t addr = state->regs[rs1] + imm; // byte address
    TRY_OR_EXIT(is_address_valid(addr), "Invalid address for LW Instruction");
    uint32_t index = addr / sizeof(uint32_t);
    state->regs[rd] = state->memory[index];
    state->pc += 4;
  } break;

  case JALR: {
    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    int32_t imm = SEXT12(insn->ops[2]);

    // compute target from register value + imm, clear low bit per spec
    uint32_t jmp_addr = state->regs[rs1] + imm;
    jmp_addr &= ~1u;

    // Treat jump to address 0 as program exit (return from main with ra=0)
    if (jmp_addr == 0) {
      state->is_running = false;
      break;
    }

    TRY_OR_EXIT(is_address_valid(jmp_addr),
                "Invalid Jump Address for JALR instruction");
    uint32_t new_ret_addr = state->pc + 4;
    if (rd != REG_x0)
      state->regs[rd] = new_ret_addr;
    state->pc = jmp_addr;
  } break;

  R_OP(XOR, ^)
  R_OP(OR,  |)
  R_OP(AND, &)
  R_OP(SLL, <<)
  R_OP(SRL, >>)

  case SRA: {
    uint32_t rd = insn->ops[0], rs1 = insn->ops[1], rs2 = insn->ops[2];
    if (rd != REG_x0)
      state->regs[rd] = (uint32_t)((int32_t)state->regs[rs1] >> state->regs[rs2]);
    state->pc += 4;
    break;
  }

  case SLTU:
  case SLT: {
    uint32_t rd = insn->ops[0], rs1 = insn->ops[1], rs2 = insn->ops[2];
    if (rd != REG_x0)
      state->regs[rd] = (insn->name == SLT)
        ? ((int32_t)state->regs[rs1] < (int32_t)state->regs[rs2])
        : (state->regs[rs1] < state->regs[rs2]);
    state->pc += 4;
    break;
  }

  default:
    assert(false && "NYI");
    break;
  }
#undef R_OP
}

// consider instructions are loaded into memory (risc_v_state).
bool emulate(risc_v_state *state) {

  state->is_running = true;
  state->is_valid = true;
  while (is_riscv_state_valid(state)) {

    // 1. Calculate the array index corresponding to the PC byte address.
    uint32_t index = state->pc / sizeof(uint32_t);
    // 2. Fetch the instruction word using the index.
    const uint32_t word = state->memory[index];
    Instruction instruction;
    if(!decode_instruction(state, word, &instruction)) {
      fprintf(stderr, "Failure decoding instruction\n");
      return false;
    }
    execute_instruction(state, &instruction);
  }

  return true;
}

// Load flat binary into emulator memory (file is a sequence of bytes)
bool load_binary(risc_v_state *state, const char *filename) {
  if (state == NULL || !filename)
    return false;
 
  FILE *f = fopen(filename, "rb");
  if (!f) return false;
  size_t max_bytes = MEMORY_SIZE * sizeof(uint32_t);
  // Read directly into state->memory; cast to uint8_t* for byte-level read
  size_t bytes_read = fread((uint8_t*)state->memory, 1, max_bytes, f);
  if (ferror(f)) {
    fclose(f);
    return false;
  }
  int extra = fgetc(f);
  if (extra != EOF) {
    // There is more data than the memory can hold
    fprintf(stderr, "Warning: binary truncated to %zu bytes\n", max_bytes);
  }
  fclose(f);
  if (bytes_read == 0) return false;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
  return true;
}

#ifndef EMU_NO_MAIN
int main(int argc, char* argv[]) {
  risc_v_state* state = NULL;
  TRY_OR_EXIT(init_riscv_emu(&state), "Failed to initialize riscv state");
  assert(state != NULL);
  if(argc > 1 )
    TRY_OR_EXIT(load_binary(state, argv[1]), "Failed to load binary");
  emulate(state);
  print_registers(state);
  print_memory(state, 0, 16);

  free(state); // cleanup

  return EXIT_SUCCESS;
}
#endif /* EMU_NO_MAIN */
