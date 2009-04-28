all: clusterer-server threads-client

#Adds the dependency of the file with the main function to the list of dependencies of each stand alone tool.
$(foreach tool,$(STANDALONE_TOOLS),$(eval $(tool): $(tool).o))

$(STANDALONE_TOOLS): $(COMMON_CPP_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)	


clusterer-server: cd server-app && make

threads-client: cd client-app && make

.PHONY: clean help

clean:
	rm -f *.o
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
