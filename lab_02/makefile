APPDIR := ./sources/
OUTDIR := ./out/
INCLUDEDIR := ./includes/

CC := gcc
FLAGS := -std=gnu99 -Wall -Werror -pedantic -c

main.out: $(OUTDIR)main.o $(OUTDIR)tree_walk.o
	$(CC) -o $@ $^

$(OUTDIR)main.o: main.c $(INCLUDEDIR)tree_walk.h
	$(CC) $(FLAGS) $< -o $@

$(OUTDIR)tree_walk.o: $(APPDIR)tree_walk.c $(INCLUDEDIR)tree_walk.h
	$(CC) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUTDIR)*.o *.out
