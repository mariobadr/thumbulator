#include "thumbulator/memory.hpp"
#include "cpu_flags.hpp"
#include "trace.hpp"
#include "utils.hpp"

namespace thumbulator {

///--- Branch operations --------------------------------------------///

// JNE - jump if not equal
uint32_t jne(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jne 0x%08X\n", decoded->offset);

  uint32_t z = cpu_get_flag_z();
  if(0==z) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JEQ - jump if equal
uint32_t jeq(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jeq 0x%08X\n", decoded->offset);

  uint32_t z = cpu_get_flag_z();
  if(1==z) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JNC - jump if no carry
uint32_t jnc(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jnc 0x%08X\n", decoded->offset);

  uint32_t c = cpu_get_flag_c();
  if(0==c) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JC - jump if carry
uint32_t jc(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jc 0x%08X\n", decoded->offset);

  uint32_t c = cpu_get_flag_c();
  if(1==c) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JN - jump if negative
uint32_t jn(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jn 0x%08X\n", decoded->offset);

  uint32_t n = cpu_get_flag_n();
  if(1==n) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JGE - jump if greater or equal (signed)
uint32_t jge(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jge 0x%08X\n", decoded->offset);

  uint32_t n = cpu_get_flag_n();
  uint32_t v = cpu_get_flag_v();
  if(0==(n^v)) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JL - jump if less (signed)
uint32_t jl(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jl 0x%08X\n", decoded->offset);

  uint32_t n = cpu_get_flag_n();
  uint32_t v = cpu_get_flag_v();
  if(1==(n^v)) {
    int32_t offset = decoded->offset * 2;
    uint32_t result = offset + cpu_get_pc();
    cpu_set_pc(result);
    BRANCH_WAS_TAKEN = 1;
  }
  return getJumpCycleCount(decoded);
}

// JMP - Unconditional branch
uint32_t jmp(decode_result const *decoded)
{
  TRACE_INSTRUCTION("jmp 0x%08X\n", decoded->offset);

  int32_t offset = decoded->offset * 2;
  uint32_t result = offset + cpu_get_pc();
  cpu_set_pc(result);
  BRANCH_WAS_TAKEN = 1;

  return getJumpCycleCount(decoded);
}

}
