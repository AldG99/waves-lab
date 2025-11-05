# Wave Simulator Makefile
# Supports both console version and GUI version with Qt5

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -Isrc

# Source files
CORE_SOURCES = src/WaveFunction.cpp src/WaveEngine.cpp src/FourierAnalyzer.cpp src/InterferenceCalculator.cpp
CONSOLE_SOURCES = $(CORE_SOURCES) src/main.cpp
GUI_SOURCES = $(CORE_SOURCES) src/MainWindow.cpp src/WaveVisualizer.cpp src/main_gui.cpp

# Object files
CORE_OBJECTS = $(CORE_SOURCES:.cpp=.o)
CONSOLE_OBJECTS = $(CONSOLE_SOURCES:.cpp=.o)
GUI_OBJECTS = $(GUI_SOURCES:.cpp=.o)

# Targets
CONSOLE_TARGET = wave-simulator
GUI_TARGET = wave-simulator-gui

# Qt5 settings (for GUI version)
# Detect OS for Qt5 configuration
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
    # macOS with Homebrew Qt5
    QT5_PREFIX = /opt/homebrew/opt/qt@5
    QT5_CFLAGS = -I$(QT5_PREFIX)/include -I$(QT5_PREFIX)/include/QtWidgets -I$(QT5_PREFIX)/include/QtCore -I$(QT5_PREFIX)/include/QtGui -std=c++17
    QT5_LIBS = -F$(QT5_PREFIX)/lib -framework QtWidgets -framework QtCore -framework QtGui
    MOC = $(QT5_PREFIX)/bin/moc
else
    # Linux with pkg-config
    QT5_CFLAGS = $(shell pkg-config --cflags Qt5Widgets Qt5Core)
    QT5_LIBS = $(shell pkg-config --libs Qt5Widgets Qt5Core)
    MOC = moc
endif

# Default target
.PHONY: all console gui clean install uninstall help

all: console

# Console version (no GUI dependencies required)
console: $(CONSOLE_TARGET)

$(CONSOLE_TARGET): $(CONSOLE_OBJECTS)
	@echo "Linking console version..."
	$(CXX) $(CONSOLE_OBJECTS) -o $(CONSOLE_TARGET) -lm

# GUI version (requires Qt5)
gui: $(GUI_TARGET)

$(GUI_TARGET): check-qt5 $(GUI_OBJECTS)
	@echo "Linking GUI version..."
	$(CXX) $(GUI_OBJECTS) -o $(GUI_TARGET) $(QT5_LIBS) -lm

# Check if Qt5 is available
check-qt5:
	@echo "Checking Qt5 dependencies..."
ifeq ($(UNAME_S), Darwin)
	@if [ ! -d "$(QT5_PREFIX)" ]; then echo "Error: Qt5 not found. Install with: brew install qt@5"; exit 1; fi
else
	@pkg-config --exists Qt5Widgets Qt5Core || (echo "Error: Qt5 development packages not found. Install qt5-default libqt5widgets5-dev libqt5core5a" && exit 1)
endif
	@echo "Qt5 found, proceeding with GUI build..."

# Object file compilation rules
src/%.o: src/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# GUI object files need Qt5 flags
src/MainWindow.o: src/MainWindow.cpp
	@echo "Compiling GUI component $<..."
	$(CXX) $(CXXFLAGS) $(QT5_CFLAGS) $(INCLUDES) -c $< -o $@

src/WaveVisualizer.o: src/WaveVisualizer.cpp
	@echo "Compiling GUI component $<..."
	$(CXX) $(CXXFLAGS) $(QT5_CFLAGS) $(INCLUDES) -c $< -o $@

src/main_gui.o: src/main_gui.cpp
	@echo "Compiling GUI main $<..."
	$(CXX) $(CXXFLAGS) $(QT5_CFLAGS) $(INCLUDES) -c $< -o $@

# Testing
test: console
	@echo "Running basic tests..."
	./$(CONSOLE_TARGET)

# Installation
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/wave-simulator

install: console
	@echo "Installing Wave Simulator..."
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(DATADIR)/examples
	install -m 755 $(CONSOLE_TARGET) $(DESTDIR)$(BINDIR)
	install -m 644 examples/* $(DESTDIR)$(DATADIR)/examples/
	@if [ -f $(GUI_TARGET) ]; then \
		install -m 755 $(GUI_TARGET) $(DESTDIR)$(BINDIR); \
		echo "Both console and GUI versions installed."; \
	else \
		echo "Console version installed. Run 'make gui install' for GUI version."; \
	fi

install-gui: gui install
	install -m 755 $(GUI_TARGET) $(DESTDIR)$(BINDIR)

uninstall:
	@echo "Uninstalling Wave Simulator..."
	rm -f $(DESTDIR)$(BINDIR)/$(CONSOLE_TARGET)
	rm -f $(DESTDIR)$(BINDIR)/$(GUI_TARGET)
	rm -rf $(DESTDIR)$(DATADIR)

# Debugging
debug: CXXFLAGS += -DDEBUG -g3
debug: clean console

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(CONSOLE_TARGET)

# Documentation
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile 2>/dev/null || echo "Doxygen not found, skipping documentation generation"

# Packaging
dist: clean
	@echo "Creating distribution package..."
	tar -czf wave-simulator-$(shell date +%Y%m%d).tar.gz \
		src/ examples/ tests/ Makefile README.md SIMULADOR_ONDAS.md

# Performance profiling
profile: CXXFLAGS += -pg
profile: clean console
	@echo "Built with profiling enabled. Run program then use 'gprof $(CONSOLE_TARGET) gmon.out'"

# Static analysis
analyze:
	@echo "Running static analysis..."
	cppcheck --enable=all --std=c++17 src/ 2>/dev/null || echo "cppcheck not found"
	
# Code formatting
format:
	@echo "Formatting code..."
	find src/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i 2>/dev/null || echo "clang-format not found"

# Clean up
clean:
	@echo "Cleaning up..."
	rm -f $(CONSOLE_OBJECTS) $(GUI_OBJECTS)
	rm -f $(CONSOLE_TARGET) $(GUI_TARGET)
	rm -f src/*.o
	rm -f gmon.out
	rm -rf docs/

# Help
help:
	@echo "Wave Simulator Build System"
	@echo "=========================="
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build console version (default)"
	@echo "  console    - Build console version only"
	@echo "  gui        - Build GUI version (requires Qt5)"
	@echo "  test       - Build and run basic tests"
	@echo "  debug      - Build with debug symbols"
	@echo "  install    - Install to system (default: /usr/local)"
	@echo "  uninstall  - Remove from system"
	@echo "  clean      - Remove all build files"
	@echo "  docs       - Generate documentation (requires Doxygen)"
	@echo "  format     - Format code (requires clang-format)"
	@echo "  analyze    - Run static analysis (requires cppcheck)"
	@echo "  dist       - Create distribution package"
	@echo "  help       - Show this help"
	@echo ""
	@echo "Dependencies for GUI version:"
	@echo "  Qt5:         sudo apt-get install qt5-default libqt5widgets5-dev"
	@echo "  QCustomPlot: sudo apt-get install libqcustomplot-dev"
	@echo ""
	@echo "Optional tools:"
	@echo "  Doxygen:     sudo apt-get install doxygen"
	@echo "  Valgrind:    sudo apt-get install valgrind"
	@echo "  Cppcheck:    sudo apt-get install cppcheck"
	@echo "  Clang-format: sudo apt-get install clang-format"

# Dependencies
$(CONSOLE_OBJECTS): src/PhysicsConstants.h
src/WaveEngine.o: src/WaveFunction.h
src/FourierAnalyzer.o: src/PhysicsConstants.h
src/InterferenceCalculator.o: src/WaveFunction.h src/PhysicsConstants.h
src/main.o: src/WaveFunction.h src/WaveEngine.h src/FourierAnalyzer.h src/InterferenceCalculator.h