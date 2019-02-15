# This makefile can help you to build test inputs for your deadlocker tool
# and check its results. If you wish to compute results using your tool as
# well, you must define the DEADLOCKER variable below to contain the path
# to your compiled deadlocker binary. If you wish to compile your own tests,
# set the paths for CLANG and OPT, as well.
#
# To build LLVM assembly files from C source files:
#   make llvmasm
#
# To analyze the inputs using your tool:
#   make analyze
#
# To remove previous output & intermediate files:
#   make clean
#

DEADLOCKER   := ../build/tools/deadlocker/deadlocker
CLANG        := clang
OPT          := opt
RM           := rm
SOURCE_FILES := $(sort $(wildcard c/*.c))
ASM_FILES    := $(addprefix ll/,$(notdir $(SOURCE_FILES:.c=.ll)))
TXT_FILES    := $(addprefix txt/,$(notdir $(ASM_FILES:.ll=.txt)))
BINARIES     := $(addprefix bin/,$(notdir $(ASM_FILES:.ll=)))


all: DIRS $(TXT_FILES)
	more txt/* | cat
llvmasm: $(ASM_FILES)
analyze: $(TXT_FILES)
bin: $(BINARIES)

DIRS:
	@mkdir -p ll
	@mkdir -p txt


ll/%.ll: c/%.c
	$(CLANG) -g -emit-llvm -O2 -mllvm -disable-llvm-optzns -S $< -o - | $(OPT) -mem2reg -S -o $@

txt/%.txt: ll/%.ll
	$(DEADLOCKER) $< > $@

bin/%: ll/%.ll
	$(CLANG) -g $< -o $@ -pthreads

clean:
	$(RM) -rf ll/ txt/
