#include "moonchild.h"

#include <stdio.h>
#include <stdlib.h>


static void progmem_cpy(void * dest, PGMEM_ADDRESS src, uint16_t size, uint16_t offset = 0) {
  char * cdest = (char *)dest;

  for (uint16_t index = 0; index < size; ++index) {
    cdest[index] = progmem_read(src, index + offset);
  }
}

static moon_closure * create_closure() {
  return (moon_closure *) malloc(sizeof(moon_closure));
}

static moon_prototype * create_prototype() {
  return (moon_prototype *) malloc(sizeof(moon_prototype));
}

static void set_to_nil(moon_reference * reference) {
  reference->progmem = TRUE;
  reference->value_addr = (SRAM_ADDRESS) &MOON_NIL_VALUE;
}

static void copy_reference(moon_reference * dest, moon_reference * src) {
  dest->progmem = src->progmem;
  dest->value_addr = src->value_addr;
}

static void load_constant(moon_reference * dest, moon_prototype * prototype, uint16_t index) {
  moon_reference constant_reference;

  progmem_cpy(&constant_reference, prototype->constants_addr, sizeof(constant_reference) * index);
  copy_reference(dest, &constant_reference);
}

static void create_register(moon_closure * closure, uint16_t index) {
  if (closure->registers[index] != NULL) return;

  closure->registers[index] = (moon_reference *) malloc(sizeof(moon_reference));
  set_to_nil(closure->registers[index]);
}

static void init_registers(moon_closure * closure) {
  for (uint16_t index = 0; index < MOON_MAX_REGISTERS; ++index) {
    closure->registers[index] = NULL;
  }
}

static void init_prototype(moon_prototype * prototype, PGMEM_ADDRESS prototype_addr) {
  progmem_cpy(prototype, prototype_addr, sizeof(moon_prototype));
}

static void init_closure(moon_closure * closure, PGMEM_ADDRESS prototype_addr) {
  closure->prototype = create_prototype();
  init_prototype(closure->prototype, prototype_addr);
  init_registers(closure);
}


static void read_instruction(moon_instruction * instruction, moon_prototype * prototype, uint16_t index) {
  progmem_cpy(instruction, prototype->instructions_addr, sizeof(moon_instruction), sizeof(moon_instruction) * index);
}

static void run_instruction(moon_closure * closure, uint16_t index) {
  moon_instruction instruction;

  read_instruction(&instruction, closure->prototype, index);

  switch(instruction.opcode) {
    case OPCODE_LOADK:
      break;

    default:
      break;
  };
}


void moon_run(PGMEM_ADDRESS prototype_addr, char * result) {
  moon_closure * closure = create_closure();

  init_closure(closure, prototype_addr);

  for (uint16_t index = 0; index < closure->prototype->instructions_count; ++index) {
    run_instruction(closure, index);
  }

  create_register(closure, 0);

  if (closure->registers[0]->progmem == TRUE) {
    sprintf(result, "yay");
  } else {
    sprintf(result, "too bad");
  }

}
