#!/usr/bin/env python3

import sys
from enum import Enum, auto

if (len(sys.argv) < 2):
	print("Command format:\n")
	print("python readbytecode.py <input_file> [output_file]")

# Dictionary of bytecode instructions
instruct = {
	0xA : "exec\n",
	0xB : "text\n",
	0xC : "data\n",
	0xD : "string",
	0xE : "move",
	0xF : "reg0,",
	0x10 : "reg1,",
	0x11 : "reg2,",
	0x12 : "reg3,",
	0x13 : "reg4,",
	0x14 : "integer",
	0x15 : "res\n",
	0x16 : "stack\n",
	0x17 : "push",
	0x18 : "pop",
	0x19 : "ret\n",
	0x1A : "exit\n",
	0x1B : "bool",
	0x1C : "call",
	0x1D : "void",
	0x40 : "read\n",
	0x41 : "write\n"
}

class Type(Enum):
	DIGIT = auto()
	INSTR = auto()
	CALL = auto()
	ASCII = auto()

def classify(byte):
	if byte <= 0x9:
		return Type.DIGIT
	elif byte <= 0x3F:
		return Type.INSTR
	elif byte <= 0x7F:
		return Type.CALL
	elif byte <= 0xFF:
		return Type.ASCII
	else:
		print("Error: illegal instruction", byte)
		exit(1)


cont = []
# Characters that get a \n after the expression
spec = ["push", "pop", "void", "integer", "string", "bool"]

with open(sys.argv[1], 'rb') as bytecode:
	for code in bytecode.read():
		byte = code & 0xFF
		bytetype = classify(byte)

		if bytetype == Type.DIGIT:
			cont.append(chr(byte + ord('0')) + "\n")
		elif (bytetype == Type.INSTR) or (bytetype == Type.CALL):
			cont.append(instruct[byte])
			if instruct[byte][-1] != '\n':
				cont.append(" ")
		elif bytetype == Type.ASCII:
			cont.append(chr(byte - 0x80))
			
			brack = False
			if (len(cont) > 5): #Check if writing to register
				brack = (chr(byte - 0x80) ==']' and ("reg" in cont[-5] or cont[-5] in spec))
			
			if brack or ((chr(byte - 0x80) == '"') and (cont[-2] != ' ')):
				cont.append("\n")
			elif chr(byte - 0x80) == ']':
				cont.append(' ')
			elif chr(byte - 0x80) == '}':
				cont.append('\n')

	print(''.join(cont))

if len(sys.argv) == 3:
	with open(sys.argv[2], 'w') as outfile:
		outfile.write(''.join(cont))

	print("Written to file \"{0}\"".format(sys.argv[2]))