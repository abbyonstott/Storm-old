'''
readbytecode.py - Print the bytecode in human readable format
Copyright (C) 2020 Ethan Onstott

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
'''

import sys
from enum import Enum

if (len(sys.argv) != 3):
	print("Command format:\n")
	print("python readbytecode.py <input_file> <output_file>")

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
	0x40 : "read\n",
	0x41 : "write\n"
}

class Type(Enum):
	DIGIT = 0
	INSTR = 1
	CALL = 2
	ASCII = 3

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
				brack = (chr(byte - 0x80) ==']' and ("reg" in cont[-5] or cont[-5] == 'pop'))


			if brack or ((chr(byte - 0x80) == '"') and (cont[-2] != ' ')):
				cont.append("\n")
			elif (chr(byte - 0x80) == ']'):
				cont.append(' ')
			elif (chr(byte - 0x80) == '}'):
				cont.append('\n')

	print(''.join(cont))

with open(sys.argv[2], 'w') as outfile:
	outfile.write(''.join(cont))

print("Written to file \"{0}\"".format(sys.argv[2]))