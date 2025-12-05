#include "emu.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
I'm looking for a couple of things...

    - I wanna exprese the number of registers in my emulator
    something like uint32_t reg[32]
    - document all of the opcodes I'll need to take into consideration
    - keep strack of a stack of a specific size  (not using it for now)

    - keep track of a program counter
        - The pc will hold the instruction to fetch
    - Memory -> uint8_t memory[MEMORY SIZE]
        - The memory will hold all of the series of instructions
           in out program
*/

bool init_riscv_emu(risc_v_state *state) {

  if (!state)
    return false;
  for (int i = 0; i < REG_COUNT; ++i)
    state->regs[i] = 0;
  for (int i = 0; i < MEMORY_SIZE; ++i)
    state->memory[i] = 0;
  state->pc = 0;
  return true;
}

bool is_address_valid(const uint32_t addr) {
  if (addr % sizeof(uint32_t) != 0)
    return false;
  uint32_t index = addr / sizeof(uint32_t);
  return index < MEMORY_SIZE;
}

bool decode_instruction(risc_v_state *state, const uint32_t instruction,
                        Instruction *output) {
  if (!state || !output)
    return false;

  uint32_t opcode = ((instruction >> 0) & 0x7F);
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
        output->name = ADD;
      else
        output->name = SUB;
    } break;

    default:
      return false;
    }
    assert((rd < REG_COUNT && rs1 < REG_COUNT && rs2 < REG_COUNT) &&
           "Invalid register val");
    output->ops[0] = rd;
    output->ops[1] = rs1;
    output->ops[2] = rs2;
    return true; // success
  } break;

  case I_FMT: {

    // 0x7F -> 0111 1111 | mask for (opcode) [6:0] bits
    // 0x1F -> 0001 1111 | mask for (rd) [11:7] bits
    // 0x7 -> 0000 0111 | mask for (funct3) [14:12] bits
    // 0x1F -> 0001 1111 | mask for (rs1) [19:15] bits
    // ((instruction >> initial_bit_position) & (bit_width) ) = ((instruction >>
    // 20) && 0xFFF)
    uint32_t rd = ((instruction >> 7) & 0x1F);
    uint32_t funct3 = ((instruction >> 12) & 0x7);
    uint32_t rs1 = ((instruction >> 15) & 0x1F);
    uint32_t imm = (instruction >> 20 & 0xFFF);

    switch (funct3) {

    case 0x00: { // addi
      output->name = ADDI;
    } break;
    default:
      assert(false && "I-FMT instruction not implemented");
    }

    output->ops[0] = rd;
    output->ops[1] = rs1;
    output->ops[2] = imm;
    return true; // success
    assert((rd < REG_COUNT && rs1 < REG_COUNT) && "Invalid register val");

  } break;

  case LOAD_FMT: {
    // this format has the same composition as I fmt
    // I-type loads (opcode 0x03)
    uint32_t rd = ((instruction >> 7) & 0x1F);
    uint32_t funct3 = ((instruction >> 12) & 0x7);
    uint32_t rs1 = ((instruction >> 15) & 0x1F);
    uint32_t imm = (instruction >> 20) & 0xFFF;

    switch (funct3) {
    case 0x2: // lw
      output->name = LW;
      break;
    default:
      return false;
    }

    output->ops[0] = rd;
    output->ops[1] = rs1;
    output->ops[2] = imm;
    return true;
  } break;

  // FIXME: As it is similar to other formats, jmp instructions share the same
  // format as I fmt, perhaps we can avoid the duplication in the bit extraction
  // mechanism we utilize below.
  case I_JMP_FMT: { // used mainly for jalr, Not to confuse with jal which has
                    // its own opcode.

    uint32_t rd = ((instruction >> 7) & 0x1F);
    uint32_t funct3 = ((instruction >> 12) & 0x7);
    uint32_t rs1 = ((instruction >> 15) & 0x1F);
    uint32_t imm = (instruction >> 20 & 0xFFF);

    if (funct3 ==
        0x00) // might not be needed but just to make sure we get it right.
      output->name = JALR;
    else
      return false;

    output->ops[0] = rd;
    output->ops[1] = rs1;
    output->ops[2] = imm;
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
      output->name = SW;
    } break;
    default:
      assert(false && "S-FMT instruction not implemented");
    }

    // ops: [rs1, rs2, imm]
    output->ops[0] = rs1;
    output->ops[1] = rs2;
    output->ops[2] = imm_final;

    return true;
  } break;
  default: {
    if (!opcode)
      return true; // temporarily, this should crash
                   // Will make it work after we figure out syscalls
  }
  }
  return false;
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
    printf("%2d  %-12s 0x%08x %10u\n", i, reg_names[i], state->regs[i],
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
  switch (insn->name) {
  case ADD: {
    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    uint32_t rs2 = insn->ops[2];
    if (rd != REG_x0) // register x0 is read only
      state->regs[rd] = state->regs[rs1] + state->regs[rs2];
    state->pc += 4;
    break;
  }
  case SUB: {
    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    uint32_t rs2 = insn->ops[2];
    if (rd != REG_x0)
      state->regs[rd] = state->regs[rs1] - state->regs[rs2];
    state->pc += 4;
    break;
  }
  case ADDI: {
    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    int32_t imm = insn->ops[2];

    // preserve signedneness on immediate
    if (imm & 0x800)     // check if the signed bit is set.
      imm |= 0xFFFFF000; // fill upper bits with 1's

    if (rd != REG_x0)
      state->regs[rd] = state->regs[rs1] + imm;
    state->pc += 4;
  } break;

  case SW: {

    // Store word: compute effective address and write rs2 value into memory
    uint32_t rs1 = insn->ops[0]; // base register
    uint32_t rs2 = insn->ops[1]; // source register (value to store)
    int32_t imm = insn->ops[2];

    // sign-extend 12-bit immediate
    if (imm & 0x800)
      imm |= 0xFFFFF000;

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
    int32_t imm = insn->ops[2];

    // sign-extend 12-bit immediate
    if (imm & 0x800)
      imm |= 0xFFFFF000;

    uint32_t addr = state->regs[rs1] + imm; // byte address
    TRY_OR_EXIT(is_address_valid(addr), "Invalid address for LW Instruction");
    uint32_t index = addr / sizeof(uint32_t);
    state->regs[rd] = state->memory[index];
    state->pc += 4;
  } break;

  case JALR: {
    uint32_t rd = insn->ops[0];
    uint32_t rs1 = insn->ops[1];
    int32_t imm = insn->ops[2];

    // preserve signedneness on immediate
    if (imm & 0x800)
      imm |= 0xFFFFF000;

    // compute target from register value + imm, clear low bit per spec
    uint32_t jmp_addr = state->regs[rs1] + imm;
    jmp_addr &= ~1u;
    TRY_OR_EXIT(is_address_valid(jmp_addr),
                "Invalid Jump Address for JALR instruction");
    uint32_t new_ret_addr = state->pc + 4;
    if (rd != REG_x0)
      state->regs[rd] = new_ret_addr;
    state->pc = jmp_addr;
  } break;

  default:
    assert(false && "NYI");
    break;
  }
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

#ifndef EMU_NO_MAIN
int main(void) {
  risc_v_state state;
  TRY_OR_EXIT(init_riscv_emu(&state), "Failed to initialize riscv state");

  // TODO: Load a program here or implement file loading
  
  print_registers(&state);

  return EXIT_SUCCESS;
}
#endif /* EMU_NO_MAIN */