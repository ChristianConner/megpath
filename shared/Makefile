CC=g++
CFLAGS=-O3 -c -std=c++11 -Wfatal-errors -Werror
LDFLAGS= 
LIBS=-lpng
SOURCES=CmdArgs.cpp Stopwatch.cpp PiecewisePF.cpp MonteAnneal.cpp MonteDebug.cpp ErrorFunctionCol.cpp ErrorFunctionRow.cpp ErrorFunction.cpp Analysis.cpp Value.cpp State.cpp HybridPF.cpp NMFMatrix.cpp FileUtil.cpp ArgFile.cpp FixedPF.cpp NormalPF.cpp ShiftPF.cpp CSVFile.cpp HistoPF.cpp ProbFunc.cpp UniformPF.cpp WeightedPF.cpp PNG.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libnmf.a

all: $(SOURCES) $(OBJECTS) $(LIBRARY)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

clean:	
	rm -f *.o 
	touch *.cpp

everything:
	$(MAKE) -C ../

cleanall:
	$(MAKE) clean -C ../
