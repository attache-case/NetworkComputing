SRC   = httpget
OBJS  = $(SRC).o http_lib.o
COMP  = g++
FLAG  = -Wall
LFLAG = -lm
LANG  = cpp

.SUFFIXES: .$(LANG).o

.$(LANG).o:
	$(COMP) -o $@ -c $(FLAG) $<

all: $(OBJS)
	$(COMP) -o $(SRC) $(OBJS) $(LFLAG)
	./$(SRC)
