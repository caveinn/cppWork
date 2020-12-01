# compiler flags, tools and include variables
GCC=g++
GCC_FLAGS=-Wall -Werror -pedantic -g
INCLUDES=-I../../include
LINKS=-L../../libs -lSimulatorException

BEAUTIFIER=uncrustify
BEAUTIFIER_FLAGS=-c ../../config/uncrustify.cfg --replace --no-backup

DOC=doxygen
DOC_FLAGS=../../config/Doxyfile

RM=rm -rf

# conditional targets, support make builds on windows or unix/linux/MacOS
ifeq ($(OS), Windows_NT)
	TEST_TARGET=test.exe
	SIM_TARGET=sim.exe
else
	TEST_TARGET=test
	SIM_TARGET=sim
endif


# objects needed to be linked together for unit test executable
test-objects = $(PROJECT_NAME)-tests.o \
              $(assg-objects)

# objects needed to be linked together for the full simulation
sim-objects = $(PROJECT_NAME)-sim.o \
                $(assg-objects)


## List of all valid targets in this project:
## ------------------------------------------
## all          : by default generate all executables
##
.PHONY : all
all : $(TEST_TARGET) $(SIM_TARGET)


## test         : Build and link together unit test executable
##
$(TEST_TARGET) : $(test-objects) $(template-files)
	$(GCC) $(GCC_FLAGS) $(test-objects) $(LINKS) -o $@

## sim          : Build and link together command line simulation
##                executable for this assignment
##
$(SIM_TARGET) : $(sim-objects) $(template-files)
	$(GCC) $(GCC_FLAGS) $(sim-objects) $(LINKS) -o $@

%.o: %.cpp
	$(GCC) $(GCC_FLAGS) $(INCLUDES) -c $< -o $@


## tests        : Run both the unit tests and the system tests together
##
.PHONY : tests
tests : unit-tests system-tests

## unit-tests   : Run the unit tests showing successful tests
##
unit-tests : $(TEST_TARGET)
	./$(TEST_TARGET) --use-colour yes

## system-tests : Run the system tests testing full simulation
##
system-tests : $(SIM_TARGET)
	./run-system-tests


## beautify     : Run the code beautifier by hand if needed
##
.PHONY : beautify
beautify :
	$(BEAUTIFIER) $(BEAUTIFIER_FLAGS) $(sources)


## docs         : Create doygen reference documentation from
##                doc comments.
##
.PHONY : docs
docs :
		@echo "Generating doxygen documentation..."
		$(DOC) $(DOC_FLAGS) 2>&1 | grep -A 1 warning | grep -v "\file statement" | grep -v "\pagebreak" | grep -v "Searching for include" | sed -e "s|${PWD}/||g"


## submit       : Create a tared and compressed file of
##                assignment files for submission
##
.PHONY : submit
submit :: beautify
	tar cvfz $(PROJECT_NAME).tar.gz $(sources)

## clean        : Remove auto-generated files for a completely
##                clean rebuild
##
.PHONY : clean
clean  :
	$(RM) $(TEST_TARGET) $(SIM_TARGET) *.o *.gch *~
	$(RM) output html latex


## help         : Get all build targets supported by this build.
##
.PHONY : help
help : Makefile ../../include/Makefile.inc
	@sed -n 's/^##//p' $^
