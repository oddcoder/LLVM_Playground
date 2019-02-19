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

PATHPROFILER := ../obj/bin/pathprofiler
CLANG        := clang
OPT          := opt
RM           := /bin/rm
CAT          := /bin/cat
SOURCE_FILES := $(sort $(wildcard c/*.c))
ASM_FILES    := $(addprefix ll/,$(notdir $(SOURCE_FILES:.c=.ll)))
TXT_FILES    := $(addprefix txt/,$(notdir $(ASM_FILES:.ll=.txt)))
BINARIES     := $(addprefix bin/,$(notdir $(ASM_FILES:.ll=)))


llvmasm: $(ASM_FILES)
analyze: $(TXT_FILES)
bin: $(BINARIES)
all: $(llvmasm)


ll/%.ll: c/%.c
	$(CLANG) -g -emit-llvm -S $< -o - | $(OPT) -mem2reg -loop-simplify -S -o $@

txt/%.txt: ll/%.ll bin/%
	$(RM) -f path-profile-results
	@echo '******** Profiling ********'
	-$(word 2,$^) 10 1 2 3 > /dev/null
	@echo '******** Analyzing ********'
	-$(PATHPROFILER) $< -p path-profile-results > $@
	-$(CAT) $@

bin/%: ll/%.ll
	@echo '******** Instrumenting ********'
	-$(PATHPROFILER) $< -o $@

clean:
	$(RM) -f $(BINARIES) $(TXT_FILES) path-profile-results

veryclean: clean
	$(RM) -f $(ASM_FILES)

