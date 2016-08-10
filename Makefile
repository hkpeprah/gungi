MFDIR := $(dir $(lastword $(MAKEFILE_LIST)))
CMAKE := cmake
BUILD := $(addprefix $(MFDIR), build)
SMDIR := $(addprefix $(MFDIR), submodules)

.PHONY: all clean

all:
	cd $(MFDIR)
	mkdir -p $(BUILD)
	cd $(BUILD) && $(CMAKE) .. && make

init:
	git submodule update --recursive

test: all
	$(BUILD)/gungi/tests/gungi-test

clean:
	-rm -rf $(BUILD)
