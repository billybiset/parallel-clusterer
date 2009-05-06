
#########Installation Parameters#################################

GROMACS_INST_PATH=/usr/local/gromacs
BOOST_INCLUDE_PATH=/usr/include/boost-1_38/

#########Compilation Flags#######################################

#includes
CPPFLAGS+= -I$(GROMACS_INST_PATH)/include/gromacs -Wall
CPPFLAGS+= -I$(BOOST_INCLUDE_PATH)
MAIN_APP_INCLUDES = -Imain-app-layer/appjobs/ -Imain-app-layer/data-definitions -Imain-app-layer/data-handling
CPPFLAGS+= -Idistributer-layer/ $(MAIN_APP_INCLUDES) -Imisc/ -Iclients-layer/ -I../common-include/ 

#linker
LDFLAGS+= -L$(GROMACS_INST_PATH)/lib/ -lgmx
LDFLAGS+= -lboost_thread-gcc43-mt 

ifeq ($(DEBUG),on)
	CPPFLAGS+=-ggdb
else
	CPPFLAGS+=-O3
endif

#########Sources#################################################

COMMON_CPP_SOURCES = \
            ../common-include/structure.cpp \
            ../common-include/client.cpp 

APPJOBS_CPP_SOURCES = \
            main-app-layer/appjobs/representatives_job.cpp 

DATA_DEFINITION_CPP_SOURCES = \
            main-app-layer/data-definitions/cluster.cpp

DATA_HANDLING_CPP_SOURCES = \
            main-app-layer/data-handling/coord3d.cpp \
            main-app-layer/data-handling/cache_structs_db.cpp \
            main-app-layer/data-handling/gmx_structs_db.cpp \
            main-app-layer/data-handling/marker_iterators.cpp \
            main-app-layer/data-handling/markers.cpp \
            main-app-layer/data-handling/rmsd.cpp \
            main-app-layer/data-handling/sequentialDB.cpp \
            main-app-layer/data-handling/marked_structuresDB.cpp \
            main-app-layer/data-handling/structuresDB.cpp

MAIN_APP_CPP_SOURCES = \
            $(DATA_DEFINITION_CPP_SOURCES) \
            $(DATA_HANDLING_CPP_SOURCES) \
            $(APPJOBS_CPP_SOURCES)

DISTRIBUTER_CPP_SOURCES = \
            distributer-layer/distributable_job.cpp \
            distributer-layer/distributer.cpp \
            distributer-layer/job_unit.cpp


CLIENTS_CPP_SOURCERS = \
            clients-layer/threads_clients_manager.cpp


CLUSTERER_CPP_SOURCES = \
            main-app-layer/clusterer_threads.cpp \
            $(COMMON_CPP_SOURCES) \
            $(MAIN_APP_CPP_SOURCES) \
            $(DISTRIBUTER_CPP_SOURCES) \
            $(CLIENTS_CPP_SOURCERS)

CLUSTERER_OBJECTS=$(patsubst %.cpp,%.o,$(CLUSTERER_CPP_SOURCES)) 

#Add to this variable any tool that will be linked with the COMMON_CPP_SOURCES objects seperated by SPACES
#Note that for this option to work there must be a source file with the name of the tool. For example
#if main_test is added to the list, there must be a main_test.cpp file to compile with the main function.
#main_test center_xtc


all: clusterer-threads

#Adds the dependency of the file with the main function to the list of dependencies of each stand alone tool.
$(foreach tool,$(STANDALONE_TOOLS),$(eval $(tool): $(tool).o))

$(STANDALONE_TOOLS): $(COMMON_CPP_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)	


clusterer-threads: $(CLUSTERER_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean help

clean: \
    for d in "clients-layer" "distributer-layer" "main-app-layer" "misc" "../common-include"; do \
        (cd $$d && rm -f *.o); \
    done #\
    #for d in "main-app-layer/appjobs" "main-app-layer/data-handling" "main-app-layer/data-definitions"; do \
    #    (cd $$d && rm -f *.o); \
    #done 
	rm -f clusterer-threads

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
