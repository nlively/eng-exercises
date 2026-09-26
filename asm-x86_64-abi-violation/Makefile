.PHONY: build run debug clean

build: exercise

exercise.o: exercise.asm
	nasm -f elf64 $< -o $@

exercise: exercise.o
	ld $< -o $@

run: exercise
	./exercise

debug: exercise
	gdb ./exercise

clean:
	rm -f exercise.o exercise
