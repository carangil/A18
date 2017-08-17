CC=gcc
A18SRC=a18.c  a18eval.c  a18util.c
A18H=a18.h
DEFS= -DMODERN $(ADEF)

a18: $(A18SRC) $(A18H)
	$(CC) -o a18 $(DEFS)  $(A18SRC) 

clean:
	rm a18
