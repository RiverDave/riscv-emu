#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

// memory limit

#define MEMORY_SIZE 4096
#define RISCV32I_STEP_COUNT 4

// Register list X-macro: name and printable string
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

#define TRY_OR_EXIT(_func, _msg)                                               \
  do {                                                                         \
    if (!(_func)) {                                                            \
      fprintf(stderr, "Error in %s at line %d: %s\n", __FILE__, __LINE__,      \
              _msg);                                                           \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

typedef struct {
  uint32_t regs[REG_COUNT];
  uint32_t memory[MEMORY_SIZE];
  uint32_t pc;
  bool is_valid;
  bool is_running;

} risc_v_state;

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

typedef enum {
  ADD,
  SUB,
} Opcode;

// source:
// https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf
enum IFormat {
  R_FMT = 0x33,
  I_FMT = 0x13,
  SB_FMT, // TODO, this whole enum might not be needed
  UJ_FMT
};

typedef struct {
  Opcode name;
  uint8_t ops[3];
} Instruction;

bool decode_instruction(risc_v_state *state, const uint32_t instruction,
                        Instruction *output) {
  if(!state || !output) return false;

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
            }
            break;

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
    default:{
        if(!opcode)
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

bool is_riscv_state_valid(const risc_v_state *state) {
  // may add more stuff in here
  return state->is_valid && state->is_running && state->pc < MEMORY_SIZE && state->memory[state->pc] != 0x00;
}

void execute_instruction(risc_v_state *state, const Instruction *insn) {
  if (!state || !insn) return;
  switch (insn->name) {
  case ADD: {
    uint8_t rd  = insn->ops[0];
    uint8_t rs1 = insn->ops[1];
    uint8_t rs2 = insn->ops[2];
    if (rd != REG_x0)
      state->regs[rd] = state->regs[rs1] + state->regs[rs2];
    state->pc+=4;
    break;
  }
  case SUB: {
    uint8_t rd  = insn->ops[0];
    uint8_t rs1 = insn->ops[1];
    uint8_t rs2 = insn->ops[2];
    if (rd != REG_x0)
      state->regs[rd] = state->regs[rs1] - state->regs[rs2];
    state->pc+=4;
    break;
  }
  default:
    break;
  }
}

// consider instructions are loaded into memory (risc_v_state).
bool emulate(risc_v_state *state) {

  state->is_running = true;
  state->is_valid = true;
  while (is_riscv_state_valid(state)){

    // 1. Calculate the array index corresponding to the PC byte address.
    uint32_t index = state->pc / sizeof(uint32_t); 
    // 2. Fetch the instruction word using the index.
    const uint32_t word = state->memory[index];
    Instruction instruction;
    TRY_OR_EXIT(decode_instruction(state, word, &instruction), "Failed to decode instruction");
    execute_instruction(state, &instruction);
  }

    return true;
}

//temporary function for testing
void load_test_add_program(risc_v_state *state, const uint8_t mem_offset) {
  if (!state) return;
  // Encoding for: add x3, x1, x2  -> 0x002081B3
  const uint32_t add_x3_x1_x2 = 0x002081B3u; // 0000(0) 0000(0)  0010(2) 0000(0) 1000(8) 0001(1) 1011(B) 0011(3)
  // 0000000 -> 0x20 -> funct7 = ADD
  
  state->memory[0 + mem_offset] = add_x3_x1_x2; // word at index 0
  state->regs[REG_x1] = 5;
  state->regs[REG_x2] = 7;
  state->pc = 0 + mem_offset;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_sub_program(risc_v_state *state, const uint8_t* passed_mem_offset) {
  uint8_t offset = 0; if(passed_mem_offset) offset = *passed_mem_offset;
  if (!state) return;
  // Encoding for: add x3, x1, x2  -> 0x002081B3
  const uint32_t sub_x3_x1_x2 = 0x202081B3u; // 0010(2) 0000(0)  0010(2) 0000(0) 1000(8) 0001(1) 1011(B) 0011(3)
  // 0010000 -> 0x20 -> funct7 = SUB
  
  state->memory[offset] = sub_x3_x1_x2; // word at index 0
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 5;
  state->pc = offset;
  state->is_valid = true;
  state->is_running = true;
}



//temporary functions for testing other R-type operations
void load_test_xor_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: xor x3, x1, x2  -> 0x0020C1B3
  const uint32_t xor_x3_x1_x2 = 0x0020C1B3u; //  0010(2) 0000(0) 1100(C) 0001(1) 1011(B) 0011(3)
  state->memory[0] = xor_x3_x1_x2;
  state->regs[REG_x1] = 10; // 0b1010
  state->regs[REG_x2] = 12; // 0b1100
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_or_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: or x3, x1, x2  -> 0x0020E1B3
  const uint32_t or_x3_x1_x2 = 0x0020E1B3u;
  state->memory[0] = or_x3_x1_x2;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 12;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_and_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: and x3, x1, x2  -> 0x0020F1B3
  const uint32_t and_x3_x1_x2 = 0x0020F1B3u;
  state->memory[0] = and_x3_x1_x2;
  state->regs[REG_x1] = 10;
  state->regs[REG_x2] = 12;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_sll_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: sll x3, x1, x2  -> 0x002091B3
  const uint32_t sll_x3_x1_x2 = 0x002091B3u;
  state->memory[0] = sll_x3_x1_x2;
  state->regs[REG_x1] = 3; // value to shift
  state->regs[REG_x2] = 1; // shift amount (lower 5 bits)
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_srl_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: srl x3, x1, x2  -> 0x0020D1B3
  const uint32_t srl_x3_x1_x2 = 0x0020D1B3u;
  state->memory[0] = srl_x3_x1_x2;
  state->regs[REG_x1] = 8; // value to shift
  state->regs[REG_x2] = 1; // shift amount
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_sra_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: sra x3, x1, x2  -> 0x4020D1B3
  const uint32_t sra_x3_x1_x2 = 0x4020D1B3u;
  state->memory[0] = sra_x3_x1_x2;
  state->regs[REG_x1] = 0xFFFFFFF8u; // -8 in two's complement
  state->regs[REG_x2] = 1; // shift amount
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_slt_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: slt x3, x1, x2  -> 0x0020A1B3
  const uint32_t slt_x3_x1_x2 = 0x0020A1B3u;
  state->memory[0] = slt_x3_x1_x2;
  state->regs[REG_x1] = 0xFFFFFFFFu; // -1 (signed)
  state->regs[REG_x2] = 1;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test_sltu_program(risc_v_state *state) {
  if (!state) return;
  // Encoding for: sltu x3, x1, x2  -> 0x0020B1B3
  const uint32_t sltu_x3_x1_x2 = 0x0020B1B3u;
  state->memory[0] = sltu_x3_x1_x2;
  state->regs[REG_x1] = 1u;
  state->regs[REG_x2] = 2u;
  state->pc = 0;
  state->is_valid = true;
  state->is_running = true;
}

void load_test(risc_v_state* state){
  // we'd load all tests so so far...
  uint8_t* mem_offset = malloc(sizeof(uint8_t));
  *mem_offset = 0;
  load_test_sub_program(state, mem_offset);
}


int main(void) {
  risc_v_state state;
  TRY_OR_EXIT(init_riscv_emu(&state), "Failed to initialize riscv state");
  load_test(&state);
  emulate(&state);
  
  print_registers(&state);

  return EXIT_SUCCESS;
}