"""
Simple helper to build a flat RISC-V binary from a C or assembly source file.

Usage:
	python get_binary.py <input.c|input.s> [output.bin]

The script will emit an ELF file (mapped to text base 0x0) and
convert it into a flat binary using objcopy.

It assumes the canonical RISC-V cross toolchain commands are available on PATH:
  - riscv64-unknown-elf-gcc
  - riscv64-unknown-elf-as
  - riscv64-unknown-elf-ld
  - riscv64-unknown-elf-objcopy
"""

import subprocess
import sys
import os


# couple of tmp fname
TMP_O = "_tmp.o"
TMP_ELF = "_tmp.elf"

def clean_tmp():
  for fname in [TMP_O, TMP_ELF]:
    if os.path.exists(fname):
        os.remove(fname)
        print(f"File '{fname}' has been deleted.")
    else:
        print(f"The file '{fname}' does not exist.")


def die(msg: str, rc: int = 1) -> None:
	print(msg, file=sys.stderr)
	sys.exit(rc)



def run_and_check(cmd: list, **kwargs):
	print("+ ", " ".join(cmd))
	al = subprocess.run(cmd, check=True, **kwargs)


def run_and_capture(cmd: list, shell: bool = False):
	"""Run cmd and capture stdout/stderr (don't raise on failure)."""
	try:
		print("+ ", " ".join(cmd) if not shell else cmd)
		r = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=shell)
		print(r.stdout.decode(errors='replace'))
		if r.stderr:
			print(r.stderr.decode(errors='replace'), file=sys.stderr)
	except Exception as e:
		print(f"Failed to run: {cmd}: {e}", file=sys.stderr)


def main(argv):
	if len(argv) < 2:
		die("usage: get_binary.py <input.c|input.s> [output.bin]")

	src_path = argv[1]
	if not os.path.exists(src_path):
		die(f"input file not found: {src_path}")

	out_bin = argv[2] if len(argv) >= 3 else "simple.bin"

	# Compile to object if needed
	_, ext = os.path.splitext(src_path)
	if ext in (".c", ".S", ".s"):
		# use cross gcc to create an object (RV32I, ilp32 ABI)
		cmd = [
			"riscv64-unknown-elf-gcc",
			"-march=rv32i",
			"-mabi=ilp32",
			"-c",
			"-O0",
			"-ffreestanding",
			"-nostdlib",
			"-o",
			TMP_O,
			src_path,
		]
		run_and_check(cmd)


	else:
		die("unsupported input file type")

	# Link object into ELF with .text at 0x0
	cmd = [
		"riscv64-unknown-elf-ld",
		"-m",
		"elf32lriscv",
		"-Ttext=0x0",
		"-o",
		TMP_ELF,
		TMP_O,
	]
	run_and_check(cmd)

	# Convert to flat binary
	cmd = [
		"riscv64-unknown-elf-objcopy",
		"-O",
		"binary",
		TMP_ELF,
		out_bin,
	]
	run_and_check(cmd)

	print(f"Wrote: {out_bin}")

	# Diagnostics
	try:
		run_and_check(["file", out_bin])
	except Exception:
		pass

	try:
		# Print a short hex view
		run_and_capture(["hexdump", "-C", out_bin])
	except Exception:
		pass

	try:
		# Disassemble the linked ELF (useful to confirm instructions)
		run_and_capture(["riscv64-unknown-elf-objdump", "-d", TMP_ELF])
	except Exception:
		pass

	try:
		# Disassemble the flat binary (tell objdump it's a binary)
		run_and_capture([
			"riscv64-unknown-elf-objdump",
			"-D",
			"-b",
			"binary",
			"-m",
			"riscv",
			"--adjust-vma=0x0",
			out_bin,
		])
	except Exception:
		pass

	clean_tmp()
  

if __name__ == "__main__":
	try:
		main(sys.argv)
	except subprocess.CalledProcessError as e:
		die(f"command failed: {e}", e.returncode)
