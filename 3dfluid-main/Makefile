CPP = g++ -Wall -pg -O2 -Ofast
SRCS = main.cpp fluid_solver.cpp EventManager.cpp

all:
	$(CPP) $(SRCS) -o fluid_sim

assembly:
	$(CPP) -S $(SRCS)
	
clean:
	@echo Cleaning up...
	@rm fluid
	@echo Done.
