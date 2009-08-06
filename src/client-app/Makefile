
#########Installation Parameters#################################

BOOST_INCLUDE_PATH=/usr/include/boost-1_38/

#########Compilation Flags#######################################

#includes
CPPFLAGS+= -I$(BOOST_INCLUDE_PATH)
CPPFLAGS+= -Iapp-clients/ -Imanagement-layer/ -Idistribution-layer/ -I../common-include/ -I. #-std=gnu++0x

#linker
LDFLAGS+= -lboost_thread-gcc43-mt
LDFLAGS+= -lboost_system-gcc43-mt

ifeq ($(DEBUG),on)
    CPPFLAGS+=-ggdb
else
    CPPFLAGS+=-O3 -Wall
endif

ifeq ($(EFFECTIVE),on)
    CPPFLAGS+=-Weffc++
endif

#########Sources#################################################

COMMON_CPP_SOURCES = \
            ../common-include/job_unit.cpp \
            ../common-include/serializable_object.cpp \
            ../common-include/binstream.cpp \
            ../common-include/string_message.cpp

APPCLIENTS_CPP_SOURCES = \
            app-clients/counter_processor.cpp

DISTRIBUTION_CPP_SOURCES = \
            distribution-layer/async_io_distribution.cpp

MANAGEMENTE_CPP_SOURCES = \
            management-layer/client_processor.cpp \
            management-layer/processors_manager.cpp

CLIENT_CPP_SOURCES = \
            counter_client.cpp \
            $(COMMON_CPP_SOURCES) \
            $(APPCLIENTS_CPP_SOURCES) \
            $(MANAGEMENTE_CPP_SOURCES) \
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
