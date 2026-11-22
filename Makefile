# Host builds: terminal simulator and core tests.
#   make        build both
#   make sim    build and run the simulator (Ace-like 16x1 display)
#   make test   build and run the core tests
#   make clean  remove build/

CXXFLAGS = -std=c++17 -Wall -Wextra -g -O1 -I.

CORE_SOURCES = core/TextViewport.cpp core/KeyMapper.cpp core/FontSystem.cpp core/MessageStore.cpp
FAKE_SOURCES = backends/fake/FakeBackend.cpp
SIM_SOURCES = sim/TerminalDisplay.cpp sim/TerminalKeySource.cpp sim/main.cpp
TEST_SOURCES = tests/test_core.cpp
HEADERS = $(wildcard core/*.h backends/*/*.h sim/*.h ui/*.h)

.PHONY: all sim test clean

all: build/sim build/test_core

build/sim: $(CORE_SOURCES) $(FAKE_SOURCES) $(SIM_SOURCES) $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $(CORE_SOURCES) $(FAKE_SOURCES) $(SIM_SOURCES)

build/test_core: $(CORE_SOURCES) $(FAKE_SOURCES) $(TEST_SOURCES) $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $(CORE_SOURCES) $(FAKE_SOURCES) $(TEST_SOURCES)

sim: build/sim
	./build/sim

test: build/test_core
	./build/test_core

clean:
	rm -rf build
