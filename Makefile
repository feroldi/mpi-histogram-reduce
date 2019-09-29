TARGET=split-bins

all: $(TARGET)

$(TARGET): $(TARGET).c
	mpicc -Wall $(TARGET).c -o $(TARGET)

run:
	mpirun ./$(TARGET)
