all: helloWorld decryptRun

helloWorld.o: helloWorld.asm
	nasm -f elf64 helloWorld.asm

helloWorld: helloWorld.o
	ld -o helloWorld helloWorld.o

decryptRun: decryptRun.c
	gcc -o decryptRun decryptRun.c

clean:
	rm -f helloWorld.o
	rm -f helloWorld
	rm -f decryptRun
