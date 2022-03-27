APPDIR := ./src/
APPTREE_DIR := ../../lab_02/sources/
OUTDIR := ./out/
INCLUDEDIR := ./inc/
INCTREE_DIR := ../../lab_02/includes/

CC := gcc
FLAGS := -std=gnu99 -Wall -pedantic -c

main.out: $(OUTDIR)main.o $(OUTDIR)print.o $(OUTDIR)read.o
	$(CC) -o $@ $^

$(OUTDIR)main.o: main.c
	$(CC) $(FLAGS) $< -o $@

$(OUTDIR)print.o: $(APPDIR)print.c $(INCLUDEDIR)print.h 
	$(CC) $(FLAGS) $< -o $@


$(OUTDIR)read.o: $(APPDIR)read.c $(INCLUDEDIR)read.h 
	$(CC) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUTDIR)*.o *.out