APPDIR := ./sources/
INCLUDEDIR := ./includes/
OUTDIR := ./out/

CC := gcc
FLAGS := -std=gnu99 -Wall -Werror -pedantic -c

main.out: $(OUTDIR)main.o $(OUTDIR)daemon.o
	$(CC) -o $@ $^

$(OUTDIR)main.o: main.c $(INCLUDEDIR)daemon.h
	$(CC) $(FLAGS) $< -o $@

$(OUTDIR)daemon.o: $(APPDIR)daemon.c $(INCLUDEDIR)daemon.h
	$(CC) $(FLAGS) $< -o $@	

.PHONY: clean
clean:
	rm -rf $(OUTDIR)*.o *.out