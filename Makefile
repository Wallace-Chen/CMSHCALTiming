COMP=g++
FLAGS=-std=c++11 -std=c++1y -std=gnu++1y
ROOTINC= -I`root-config --incdir`
ROOTLIBS=  `root-config --glibs --cflags | sed 's/-pthread//g'` -lMinuit 
INC= -I./include
LIBS= ${ROOTLIBS} 
SRC= ./src
BASE_OBJECTS=$(SRC)/analysisClass.o $(SRC)/baseClass.o $(SRC)/Object.o $(SRC)/Collection.o $(SRC)/Cell.o
PHYS_OBJECTS=$(SRC)/HcalDigi.o $(SRC)/HcalDigiIDs.o $(SRC)/HcalSample.o $(SRC)/HBHEDigi.o $(SRC)/HBHESample.o 
TREE_OBJECTS=$(SRC)/HcalTupleTree.o 
OBJECTS=$(BASE_OBJECTS) $(TOOL_OBJECTS) $(PHYS_OBJECTS) $(TREE_OBJECTS)

default: main

main: $(SRC)/main.o $(OBJECTS) 
	$(COMP) $(INC) $(ROOTINC) $(LIBS) $(FLAGS) $(CFLAGS) -o $@ $(OBJECTS) $(SRC)/$@.o

.cc.o:
	$(COMP) -c $(INC) $(ROOTINC) $(FLAGS) $(CFLAGS) -o $@ $<

.C.o:
	$(COMP) -c $(INC) $(ROOTINC) $(FLAGS) $(CFLAGS) -o $@ $<

clean:
	rm -rf main
	rm -rf $(SRC)/*.o

