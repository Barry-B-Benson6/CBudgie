#pragma once
#include <string>
#include <unordered_map>
enum class Opcode {
  CALL,

  LOAD,
  LOAD_ALL,

  STORE,
  STORE_ALL,

  LOAD_NUMCONST,

  LOAD_WORDCONST,

  LOAD_BOOLCONST,

  RETURN,

  IF_JUMP,
  JUMP,

  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  POW,
  POP,

  TO_STRING,
  TO_BOOL,
  TO_NUM,

  LESS_THAN,
  GREATER_THAN,
  EQUAL,
  NOT_EQUAL,
  LESS_THAN_EQUAL,
  GREATER_THAN_EQUAL,

  AND,
  OR,
  XOR,
  NOT,

  READ,
  PRINT,

  NEWSCOPE,
  ENDSCOPE,
  NOP
};

static const std::unordered_map<Opcode, std::string> OpcodeToString = {
    {Opcode::CALL, "CALL"},
    {Opcode::LOAD, "LOAD"},
    {Opcode::LOAD_ALL, "LOAD_ALL"},
    {Opcode::STORE, "STORE"},
    {Opcode::STORE_ALL, "STORE_ALL"},
    {Opcode::LOAD_NUMCONST, "LOAD_NUMCONST"},
    {Opcode::LOAD_WORDCONST, "LOAD_WORDCONST"},
    {Opcode::LOAD_BOOLCONST, "LOAD_BOOLCONST"},
    {Opcode::RETURN, "RETURN"},
    {Opcode::IF_JUMP, "IF_JUMP"},
    {Opcode::JUMP, "JUMP"},
    {Opcode::ADD, "ADD"},
    {Opcode::SUB, "SUB"},
    {Opcode::MUL, "MUL"},
    {Opcode::DIV, "DIV"},
    {Opcode::POW, "POW"},
    {Opcode::MOD, "MOD"},
    {Opcode::POP, "POP"},
    {Opcode::TO_STRING, "TO_STRING"},
    {Opcode::TO_BOOL, "TO_BOOL"},
    {Opcode::TO_NUM, "TO_NUM"},
    {Opcode::LESS_THAN, "LESS_THAN"},
    {Opcode::GREATER_THAN, "GREATER_THAN"},
    {Opcode::EQUAL, "EQUAL"},
    {Opcode::NOT_EQUAL, "NOT_EQUAL"},
    {Opcode::LESS_THAN_EQUAL, "LESS_THAN_EQUAL"},
    {Opcode::GREATER_THAN_EQUAL, "GREATER_THAN_EQUAL"},
    {Opcode::AND, "AND"},
    {Opcode::OR, "OR"},
    {Opcode::XOR, "XOR"},
    {Opcode::NOT, "NOT"},
    {Opcode::READ, "READ"},
    {Opcode::PRINT, "PRINT"},
    {Opcode::NEWSCOPE, "NEWSCOPE"},
    {Opcode::ENDSCOPE, "ENDSCOPE"},
    {Opcode::NOP, ""}
};

static const std::unordered_map<std::string, Opcode> stringToOpcode = {
    {"CALL", Opcode::CALL},
    {"LOAD", Opcode::LOAD},
    {"LOAD_ALL", Opcode::LOAD_ALL},
    {"STORE", Opcode::STORE},
    {"STORE_ALL", Opcode::STORE_ALL},
    {"LOAD_NUMCONST", Opcode::LOAD_NUMCONST},
    {"LOAD_WORDCONST", Opcode::LOAD_WORDCONST},
    {"LOAD_BOOLCONST", Opcode::LOAD_BOOLCONST},
    {"RETURN", Opcode::RETURN},
    {"IF_JUMP", Opcode::IF_JUMP},
    {"JUMP", Opcode::JUMP},
    {"ADD", Opcode::ADD},
    {"SUB", Opcode::SUB},
    {"MUL", Opcode::MUL},
    {"DIV", Opcode::DIV},
    {"POW", Opcode::POW},
    {"MOD", Opcode::MOD},
    {"POP", Opcode::POP},
    {"TO_STRING", Opcode::TO_STRING},
    {"TO_BOOL", Opcode::TO_BOOL},
    {"TO_NUM", Opcode::TO_NUM},
    {"LESS_THAN", Opcode::LESS_THAN},
    {"GREATER_THAN", Opcode::GREATER_THAN},
    {"EQUAL", Opcode::EQUAL},
    {"NOT_EQUAL", Opcode::NOT_EQUAL},
    {"LESS_THAN_EQUAL", Opcode::LESS_THAN_EQUAL},
    {"GREATER_THAN_EQUAL", Opcode::GREATER_THAN_EQUAL},
    {"AND", Opcode::AND},
    {"OR", Opcode::OR},
    {"XOR", Opcode::XOR},
    {"NOT", Opcode::NOT},
    {"READ", Opcode::READ},
    {"PRINT", Opcode::PRINT},
    {"NEWSCOPE", Opcode::NEWSCOPE},
    {"ENDSCOPE",Opcode::ENDSCOPE},
    {"", Opcode::NOP},
    {"#", Opcode::NOP}
};
