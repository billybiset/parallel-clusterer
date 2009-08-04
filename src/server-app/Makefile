
#########Installation Parameters#################################

BOOST_INCLUDE_PATH=/usr/include/boost-1_38/

#########Compilation Flags#######################################

#includes
CPPFLAGS+= -I$(BOOST_INCLUDE_PATH)
MAIN_APP_INCLUDES = -Imain-app-layer/appjobs/ -Imain-app-layer/data-definitions -Imain-app-layer/data-handling
CPPFLAGS+= -Idistributer-layer/ $(MAIN_APP_INCLUDES) -Imisc/ -Iclients-layer/ -I../common-include/ 

#linker
LDFLAGS+= -lboost_thread-gcc43-mt 
LDFLAGS+= -lboost_system-gcc43-mt

# ifeq ($(DEBUG),on)
	CPPFLAGS+=-ggdb
# else
# 	CPPFLAGS+=-O3
# endif

#########Sources#################################################

COMMON_CPP_SOURCES = \
            ../common-include/job_unit.cpp \
            ../common-include/string_message.cpp \
            ../common-include/binstream.cpp

APPJOBS_CPP_SOURCES = \
            main-app-layer/appjobs/counter.cpp

MAIN_APP_CPP_SOURCES = \
            $(APPJOBS_CPP_SOURCES)

DISTRIBUTER_CPP_SOURCES = \
            distributer-layer/distributable_job.cpp \
            distributer-layer/distributer.cpp


CLIENTS_CPP_SOURCERS = \
            clients-layer/clients_manager.cpp \
            clients-layer/async_io_clients_manager.cpp \
            clients-layer/client_proxy.cpp


CLUSTERER_CPP_SOURCES = \
            main-app-layer/counter_main.cpp \
            $(COMMON_CPP_SOURCES) \
            $(MAIN_APP_CPP_SOURCES) \
            $(DISTRIBUTER_CPP_SOURCES) \
            $(CLIENTS_CPP_SOURCERS)

CLUSTERER_OBJECTS=$(patsubst %.cpp,%.o,$(CLUSTERER_CPP_SOURCES)) 

#Add to this variable any tool that will be linked with the COMMON_CPP_SOURCES objects seperated by SPACES
#Note that for this option to work there must be a source file with the name of the tool. For example
#if main_test is added to the list, there must be a main_test.cpp file to compile with the main function.
#main_test center_xtc


all: counter

counter: $(CLUSTERER_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: cleanall cleanobj cleanbackup help

cleanall : cleanobj cleanbackup
	rm clusterer

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
