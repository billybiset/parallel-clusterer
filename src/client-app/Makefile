
#########Installation Parameters#################################

GROMACS_INST_PATH=/usr/local/gromacs
BOOST_INCLUDE_PATH=/usr/include/boost-1_38/

#########Compilation Flags#######################################

#includes
CPPFLAGS+= -I$(GROMACS_INST_PATH)/include/gromacs -Wall
CPPFLAGS+= -I$(BOOST_INCLUDE_PATH)
CPPFLAGS+= -Iapp-clients/ -I../common-include/ -I. -std=gnu++0x

#linker
LDFLAGS+= -L$(GROMACS_INST_PATH)/lib/ -lgmx
LDFLAGS+= -lboost_thread-gcc43-mt 
LDFLAGS+= -lboost_system-gcc43-mt

ifeq ($(DEBUG),on)
	CPPFLAGS+=-ggdb
else
	CPPFLAGS+=-O3
endif

#########Sources#################################################

APPCLIENTS_CPP_SOURCES = \
            app-clients/clusterer_client.cpp

DISTRIBUTION_CPP_SOURCES = \
            distribution-layer/async_io_distribution.cpp

CLIENT_CPP_SOURCES = \
            client.cpp \
            main_clusterer_client.cpp \
            $(APPCLIENTS_CPP_SOURCES) \
            $(DISTRIBUTION_CPP_SOURCES)

CLIENT_OBJECTS=$(patsubst %.cpp,%.o,$(CLIENT_CPP_SOURCES)) 

all: client

client: $(CLIENT_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: cleanall cleanobj cleanbackup help

cleanall : cleanobj cleanbackup
	rm -f client

cleanobj :
	rm -f *.o */*.o */*/*.o

cleanbackup :
	rm -f *~ */*~ */*/*~

help:
	@echo
	@echo --------------------------------------------------------	
	@echo To compile all modules just type "make"
	@echo Current possible compilation modules are:
	@echo $(STANDALONE_TOOLS) clusterer
	@echo	
	@echo In order to turn on debugging set the environmental
	@echo variable DEBUG to on or type "make DEBUG=on <target>"
	@echo	
	@echo For information about prerequisites read the README file
	@echo --------------------------------------------------------
