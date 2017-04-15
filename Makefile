CC=gcc
A18SRC=a18.c  a18eval.c  a18util.c
A18H=a18.h
DEFS= -DMODERN

a18: $(A18SRC) $(A18H)
	$(CC) -o a18i $(DEFS)  $(A18SRC) 


