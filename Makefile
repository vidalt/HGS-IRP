OUTPUT=irp-1-ds-single-ri-ds-ri

all : $(OUTPUT)

CCC = g++
CCFLAGS = -g -std=gnu++14
# LIBS= -lconcert -lilocplex -lcplex -lm -lpthread -ldl
LIBS= -lm -lpthread -ldl
#LIBS += -L/Users/jingyi/anaconda3/lib -lpython3.11
#CPPFLAGS += -I/Users/jingyi/anaconda3/include/python3.11
#CPPFLAGS += -I/Users/jingyi/anaconda3/lib/python3.11/site-packages/numpy/core/include/
# PATHLIBS = -L/opt/ibm/ILOG/CPLEX_Studio128/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio128/concert/lib/x86-64_linux/static_pic
PATHLIBS=
TARGETDIR=.
# CPPFLAGS += -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -I/opt/ibm/ILOG/CPLEX_Studio128/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio128/concert/include
CPPFLAGS += -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD


OBJS2 = \
        $(TARGETDIR)/Client.o \
        $(TARGETDIR)/Vehicle.o \
        $(TARGETDIR)/commandline.o \
	$(TARGETDIR)/Genetic.o \
        $(TARGETDIR)/Individu.o \
        $(TARGETDIR)/LocalSearch.o \
        $(TARGETDIR)/main.o \
        $(TARGETDIR)/Noeud.o \
        $(TARGETDIR)/Rng.o \
        $(TARGETDIR)/Params.o \
        $(TARGETDIR)/Population.o \
        $(TARGETDIR)/Route.o \
        $(TARGETDIR)/Mutations.o \
        $(TARGETDIR)/LinearPiece.o \
        $(TARGETDIR)/PLFunction.o \
        $(TARGETDIR)/LotSizingSolver.o \
        
	
$(TARGETDIR)/$(OUTPUT): $(OBJS2)
	$(CCC)  $(CCFLAGS) $(CPPFLAGS) $(PATHLIBS) -o $(TARGETDIR)/$(OUTPUT) $(OBJS2) $(LIBS)


$(TARGETDIR)/LinearPiece.o: LinearPiece.h LinearPiece.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c LinearPiece.cpp -o $(TARGETDIR)/LinearPiece.o

$(TARGETDIR)/PLFunction.o: PLFunction.h PLFunction.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c PLFunction.cpp -o $(TARGETDIR)/PLFunction.o

$(TARGETDIR)/LotSizingSolver.o: LotSizingSolver.h LotSizingSolver.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c LotSizingSolver.cpp -o $(TARGETDIR)/LotSizingSolver.o

$(TARGETDIR)/Client.o: Client.h Client.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Client.cpp -o $(TARGETDIR)/Client.o

$(TARGETDIR)/Vehicle.o: Vehicle.h Vehicle.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Vehicle.cpp -o $(TARGETDIR)/Vehicle.o
	
$(TARGETDIR)/commandline.o: commandline.h commandline.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c commandline.cpp -o $(TARGETDIR)/commandline.o
	
$(TARGETDIR)/Genetic.o: Genetic.h Genetic.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Genetic.cpp -o $(TARGETDIR)/Genetic.o

$(TARGETDIR)/Individu.o: Individu.h Individu.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Individu.cpp -o $(TARGETDIR)/Individu.o

$(TARGETDIR)/LocalSearch.o: LocalSearch.h LocalSearch.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c LocalSearch.cpp -o $(TARGETDIR)/LocalSearch.o
	
$(TARGETDIR)/main.o: main.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c main.cpp -o $(TARGETDIR)/main.o
	
$(TARGETDIR)/Noeud.o: Noeud.h Noeud.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Noeud.cpp -o $(TARGETDIR)/Noeud.o

$(TARGETDIR)/SeqData.o: Rng.h Rng.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Rng.cpp -o $(TARGETDIR)/SeqData.o

$(TARGETDIR)/Params.o: Params.h Params.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Params.cpp -o $(TARGETDIR)/Params.o

$(TARGETDIR)/Population.o: Population.h Population.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Population.cpp -o $(TARGETDIR)/Population.o

$(TARGETDIR)/Route.o: Route.h Route.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Route.cpp -o $(TARGETDIR)/Route.o

$(TARGETDIR)/Mutations.o: LocalSearch.h Mutations.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Mutations.cpp -o $(TARGETDIR)/Mutations.o


clean:
	$(RM) \
	$(TARGETDIR)/irp \
	$(TARGETDIR)/Client.o \
        $(TARGETDIR)/Vehicle.o \
       $(TARGETDIR)/main.o \
     $(TARGETDIR)/commandline.o \
	 $(TARGETDIR)/Genetic.o \
     $(TARGETDIR)/Individu.o \
     $(TARGETDIR)/LocalSearch.o \
     $(TARGETDIR)/Noeud.o \
     $(TARGETDIR)/Rng.o \
     $(TARGETDIR)/Params.o \
     $(TARGETDIR)/Population.o \
     $(TARGETDIR)/Route.o \
     $(TARGETDIR)/Mutations.o \
     $(TARGETDIR)/LinearPiece.o \
     $(TARGETDIR)/PLFunction.o \
     $(TARGETDIR)/LotSizingSolver.o \



