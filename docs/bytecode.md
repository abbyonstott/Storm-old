# Byte Code
### Digits are between `0x00` and `0x09`

### VM instructions are reserved `0x0A` - `0x3F`
Instruction  | Value
---------|-----------
exec     | `0x0A`
text     | `0x0B`
data     | `0x0C`
string   | `0x0D`
move     | `0x0E`
reg0     | `0x0F`
reg1     | `0x10`
reg2     | `0x11`
reg3     | `0x12`
reg4     | `0x13`
integer  | `0x14`
res      | `0x15`
stack    | `0x16`
push     | `0x17`
pop      | `0x18`
ret      | `0x19`
exit     | `0x1A`
bool     | `0x1B`
call     | `0x1C`

### Between `0x40` and `0x7E` are call numbers
Call    |  value
--------|-----------
read    | `0x40`
write   | `0x41`

### Between `0x7F` and `0xFF` is ASCII

### Variables are given identifiers, specified with square brackets, that show where the id begins and ends as to not confuse it with normal bytecode
