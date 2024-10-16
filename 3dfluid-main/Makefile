CPP = g++ -Wall  -O2 -Ofast -march=native -ffast-math -funroll-loops -fomit-frame-pointer -flto
SRCS = main.cpp fluid_solver.cpp EventManager.cpp

all:
	$(CPP) $(SRCS) -o fluid_sim

assembly:
	$(CPP) -S $(SRCS)
	
clean:
	@echo Cleaning up...
	@rm fluid
	@echo Done.
