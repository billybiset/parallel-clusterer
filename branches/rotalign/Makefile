XDRFILE_INCLUDES= -I/usr/local/include/xdrfile/
GETOPT_INCLUDES = -I../../libraries/getopt

CLUSTERER_INCLUDES= -Icommon

LDFLAGS+= -L/usr/local/lib
LDFLAGS+= -lboost_thread-gcc43-mt
LDFLAGS+= -lboost_system-gcc43-mt
LDFLAGS+= -lxdrfile

CPPFLAGS+= -Icommon -Iserver -I/usr/local/include/xdrfile/ -DMILI_NAMESPACE -I/usr/include/boost-1_38/ -I../../libraries/getopt -Iclient/include

ifeq ($(COVER),on)
    CPPFLAGS+=-fprofile-arcs -ftest-coverage -fworking-directory
    LDFLAGS+=-lgcov
endif


ifeq ($(DEBUG),on)
    CPPFLAGS+=-ggdb3
else
    ifeq ($(NORMAL),on)
        CPPFLAGS+=-Wall
    else
        CPPFLAGS+=-O3 -Wall
    endif

endif

ifeq ($(EFFECTIVE),on)
    CPPFLAGS+=-Weffc++
endif

#########Sources###############################################
MAIN_SOURCES = \
    server/protein_database.cpp \
    server/cluster.cpp \
    server/clusterer_output.cpp \
    server/clusterer_main.cpp

JOBS_SOURCES = \
	server/jobs/adding_job.cpp \
	server/jobs/centers_job.cpp \
	server/jobs/clusters_job.cpp \
	server/jobs/representatives_job.cpp 

OUTPUT_SOURCES = \
	server/outputs/xtc_output.cpp

CLUSTERER_SOURCES = \
	$(JOBS_SOURCES) \
    $(MAIN_SOURCES) \
    $(OUTPUT_SOURCES) \
    ../../libraries/getopt/getopt_pp.cpp


CLUSTERER_OBJECTS=$(patsubst %.cpp,%.o,$(CLUSTERER_SOURCES))

###################
APPCLIENTS_CPP_SOURCES = \
            client/clusterer_processor.cpp

CLIENT_CPP_SOURCES = \
            client/clusterer_client.cpp \
            $(APPCLIENTS_CPP_SOURCES) \
            ../../libraries/getopt/getopt_pp.cpp

CLIENT_OBJECTS=$(patsubst %.cpp,%.o,$(CLIENT_CPP_SOURCES))
###################

clusterer: $(CLUSTERER_OBJECTS)
	$(CXX) -o clusterer $^ $(LDFLAGS) -lfud

clusterer-client: $(CLIENT_OBJECTS)
	$(CXX) -o clusterer-client $^ $(LDFLAGS) -lfud_client


all: clusterer clusterer-client

.PHONY: cleanall cleanobj cleanbackup help

cleanall : cleanobj cleanbackup
	rm -f clusterer clusterer-client

cleanobj :
	rm -f *.o */*.o */*/*.o */*/*/*.o

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
