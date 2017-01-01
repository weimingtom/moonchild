#include "moonchild.h"
#include "Arduboy.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>


static Arduboy arduboy;

typedef struct {
  unsigned char opcode;
} lua_instruction;

typedef struct {
  uint16_t luac_size;
  uint16_t luac;
  uint16_t cursor;
  uint16_t top_func_name_size;
  char * top_func_name;
  uint16_t instructions_count;
  lua_instruction * instructions;
  uint16_t constants_count;
  uint16_t first_constant;
} lua_program;


static unsigned char read_byte(lua_program * program) {
  unsigned char byte = pgm_read_byte_near((program->luac) + (program->cursor));

  program->cursor++;
  return byte;
}

static skip_bytes(lua_program * program, uint16_t size) {
  program->cursor += size;
}

static void read_literal(lua_program * program, char * output, uint16_t size) {
  for (uint16_t index = 0; index < size; ++index) {
    output[index] = (char *) read_byte(program);
  }

  output[size] = '\0';
}

static void read_top_func_name(lua_program * program) {
  program->top_func_name_size = (uint16_t) read_byte(program) - 1;
  program->top_func_name = (char *) malloc(program->top_func_name_size + 1);

  read_literal(program, program->top_func_name, program->top_func_name_size);  
}

static int read_int(lua_program * program) {
  int res = read_byte(program);

  skip_bytes(program, 3);

  return res;
}

static void read_code(lua_program * program) {
  program->instructions_count = read_int(program);
  program->instructions = (lua_instruction *) malloc(program->instructions_count * sizeof(lua_instruction));

  for (uint16_t index = 0; index < program->instructions_count; ++index) {
    program->instructions[index].opcode = read_byte(program);
    skip_bytes(program, 3);
  }
}

static void read_constants(lua_program * program) {
  program->constants_count = read_int(program);

  if (program->constants_count <= 0) return;

  skip_bytes(program, 1); // constant type
  program->first_constant = read_int(program);
}

void moonchild_run(uint16_t luac, uint16_t luac_size) {
  lua_program * program = (lua_program *) malloc(sizeof(lua_program));

  program->luac = luac;
  program->luac_size = luac_size;
  program->cursor = 0;

  arduboy.begin();
  arduboy.setFrameRate(15);
  
  skip_bytes(program, 33);  // header
  skip_bytes(program, 1);  // sizeup values

  read_top_func_name(program);

  skip_bytes(program, 4);  // first line defined
  skip_bytes(program, 4);  // last line defined
  skip_bytes(program, 1);  // numparams
  skip_bytes(program, 1);  // isvarargs
  skip_bytes(program, 1);  // maxstacksize

  read_code(program);
  read_constants(program);

  char buffer[255];

  sprintf(buffer, "%d", program->first_constant);


  while(1) {
    if (!(arduboy.nextFrame())) continue;

    arduboy.clear();

    arduboy.setCursor(4, 9);
    arduboy.print(buffer);

    arduboy.display();    
  }

  free(program->top_func_name);
  free(program->instructions);
  free(program);
}
