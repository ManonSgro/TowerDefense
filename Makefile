CXX        := g++
CXXFLAGS   :=
THIRD_PART := ./third-party
BUILD      := ./build
OBJ_DIR    := $(BUILD)/objects
BIN_DIR    := $(BUILD)/bin
INCLUDE    := -I./include/ -I$(THIRD_PART)/include
SRC_DIR    := ./src
OBJECTS    := $(OBJ_DIR)/main.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/map_reader.o $(OBJ_DIR)/map.o $(OBJ_DIR)/monster.o $(OBJ_DIR)/tower.o $(OBJ_DIR)/player.o $(OBJ_DIR)/installation.o $(OBJ_DIR)/button.o

ifeq ($(OS), Windows_NT)
	TARGET     := Willy.exe
	LDFLAGS    := -L$(THIRD_PART)/lib -lSDL -lSDL_image -lopengl32 -lglu32 -L$(BIN_DIR)
	MKDIR = if not exist "$(1)" mkdir "$(1)"
	CP := xcopy /i /y
	RM := rd /s /q
	CXXFLAGS   := $(CXXFLAGS)
	INSTALL := 	$(CP) /s "$(THIRD_PART)/bin" "$(BIN_DIR)/*"
else
	TARGET     := Willy.out
	MKDIR = mkdir -p $(1)
	CP := cp
	RM := rm -rvf
	LDFLAGS    := -L$(THIRD_PART)/lib -lSDL -lSDL_image -lGL -lGLU
	INSTALL :=
endif

all: $(BIN_DIR)/$(TARGET)

run: $(BIN_DIR)/$(TARGET)
	$(BIN_DIR)/$(TARGET) $(ARGS)

debug: CXXFLAGS := $(CXXFLAGS) -DDEBUG -g -O1 -Wall
debug: all

release: CXXFLAGS := $(CXXFLAGS) -DRELEASE -O3
release: all

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(call MKDIR,$(@D))
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(call MKDIR,$(BIN_DIR))
	$(INSTALL)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJECTS) $(LDFLAGS) -o $(BIN_DIR)/$(TARGET)

clean:
	$(RM) "$(OBJ_DIR)"
	$(RM) "$(BIN_DIR)"
