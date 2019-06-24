import argparse
import os.path
import os

# parser = argparse.ArgumentParser(description='Convert Binary to MIF.')
# parser.add_argument('input', help='Input bin file')

# args = parser.parse_args()

input_path = r"/mnt/c/Users/gusta/OneDrive/Arquivos da Faculdade/2017 - 1/Lab de AOC/Repositório/Verilogs/ExternalMemory/"

quartus_proj_path = r"/mnt/c/Users/gusta/OneDrive/Arquivos da Faculdade/2017 - 1/Lab de AOC/Quartus/"

proj_name = "ARMAria.qpf"

islinux = True

if( not os.path.exists(input_path) ):
    islinux = False
    quartus_proj_path = r"C:\Users\gusta\OneDrive\Arquivos da Faculdade\2017 - 1\Lab de AOC\Quartus" + "\\"
    input_path = r"C:\Users\gusta\OneDrive\Arquivos da Faculdade\2017 - 1\Lab de AOC\Repositório\Verilogs\ExternalMemory"  + "\\"

output_path = quartus_proj_path + "db/"

output_file = output_path + "ARMAria.ram0_Memory_a0c6519c.hdl.mif"

input_file = input_path + "ProgramaMIF.txt"

quartus_proj = quartus_proj_path + proj_name

file_content = '''-- begin_signature
-- Memory
-- end_signature
WIDTH=32;
DEPTH=16384;

ADDRESS_RADIX=UNS;
DATA_RADIX=BIN;

CONTENT BEGIN
'''

file_end = "END;"

instructions = []
with open(input_file, 'r') as inputFile:
    for line in inputFile:
        instructions.append("0000000000000000{}".format(line[:16]))

print("loaded {} instructions".format(len(instructions)))

for i in range(0, 16384 - len(instructions)):
    file_content += "\t{} : XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;\n".format(16383 - i)

for i, inst in enumerate(instructions[::-1]):
    file_content += "\t{} : {};\n".format(len(instructions) - i -1, inst)


file_content += file_end

with open(output_file, 'w') as output:
    output.write(file_content)

if not islinux:
    os.system("quartus_cdb.exe --update_mif \"{}\"".format(quartus_proj))
    os.system("quartus_asm.exe \"%s\"" % quartus_proj)