#XDRFILE_INCLUDES= -I/usr/local/include/xdrfile/

CLUSTERER_INCLUDES= -Icommon

LDFLAGS+= -L/usr/local/lib
LDFLAGS+= -lboost_thread-gcc43-mt
LDFLAGS+= -lboost_system-gcc43-mt
#LDFLAGS+= -lxdrfile
LDFLAGS+= -lgetopt_pp
LDFLAGS+= -lprot-filer

CPPFLAGS+= -Icommon -Iserver -DMILI_NAMESPACE -I/usr/include/boost-1_38/ -Iclient/include

ifeq ($(COVER),on)
    CPPFLAGS+=-fprofile-arcs -ftest-coverage -fworking-directory
    LDFLAGS+=-lgcov
endif

ifneq ($(ROTALIGN),off)
    CPPFLAGS+=-DROTALIGN
endif

ifneq ($(PEDANTIC),off)
    CPPFLAGS+=-pedantic -ansi
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

COMMON_SOURCES = \
    common/protein.cpp

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


CLUSTERER_SOURCES = \
	$(JOBS_SOURCES) \
    $(COMMON_SOURCES) \
    $(MAIN_SOURCES) 


CLUSTERER_OBJECTS=$(patsubst %.cpp,%.o,$(CLUSTERER_SOURCES))

###################
APPCLIENTS_CPP_SOURCES = \
            client/clusterer_processor.cpp

CLIENT_CPP_SOURCES = \
            client/clusterer_client.cpp \
            $(COMMON_SOURCES) \
            $(APPCLIENTS_CPP_SOURCES)

CLIENT_OBJECTS=$(patsubst %.cpp,%.o,$(CLIENT_CPP_SOURCES))
###################

all: clusterer clusterer-client

clusterer: $(CLUSTERER_OBJECTS)
	$(CXX) -o clusterer $^ $(LDFLAGS) -lfud

clusterer-client: $(CLIENT_OBJECTS)
	$(CXX) -o clusterer-client $^ $(LDFLAGS) -lfud_client


.PHONY: cleanall cleanobj cleanbackup help

clean : cleanobj cleanbackup
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
	@echo clusterer clusterer-client
	@echo
	@echo In order to turn on debugging set the environmental
	@echo variable DEBUG to on or type "make DEBUG=on <target>"
	@echo
	@echo For information about prerequisites read the README file
	@echo --------------------------------------------------------
