CC := g++-4.8
FLAGS := -std=c++11
INC := ./include
BIN := ./bin
BUILD := ./build
SRC := ./src
INCLUDE := -I$(INC)

$(BIN)/main: $(BUILD)/Date.o $(BUILD)/User.o \
	$(BUILD)/Meeting.o $(BUILD)/Storage.o \
	$(BUILD)/AgendaService.o $(BUILD)/AgendaUI.o $(BUILD)/Agenda.o
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@
$(BUILD)/%.o: $(SRC)/%.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c $^ -o $@
clean:
	rm $(BUILD)/*.o $(BIN)/main

