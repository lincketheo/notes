CMAKE_FLAGS :=

all: build/tx
	rm -f gmon.out
	rm -f gmon.txt

build/tx: build/Makefile
	cd build && make

build/Makefile: build
	cd build && cmake .. $(CMAKE_FLAGS)

build:
	mkdir -p build

.PHONY: clean clean-all format

clean:
	if [ -d build ]; then \
		(cd build && make clean); \
		fi
	find . -name "*.o" -type f -delete
	find . -name "*.det" -type f -delete
	find . -name "*.bin" -type f -delete
	find . -name "*.tmp" -type f -delete
	find . -name "__pycache__" -type d -delete
	rm -f gmon.*

clean-all: clean
	rm -rf build

format:
	find ./include -iname "*.h" | xargs clang-format -i
	find ./src -iname "*.cpp" | xargs clang-format -i

release:
	$(MAKE) CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=Release"

debug:
	$(MAKE) CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=Debug"
