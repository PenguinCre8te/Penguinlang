build:
	gcc ast.c compiler.c token.c vm.c
clean:
	del /Q *.exe