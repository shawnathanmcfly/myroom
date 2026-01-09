OBJS = *.o
SRCS_CPP = *.cpp
SRCS_C = *.c
DXIL_SHADERS = shaders/DXIL/*.*
SPIRV_SHADERS = shaders/SPIRV/*.*

EXE_NAME = myroom

ifeq ($(OS), Windows_NT)
$(info Operating system is Windows)
WIN = -Wl,-subsystem,windows -lmingw32 -lSDL2main
INC = "-IC:\mingw64\include"
LIB = "-LC:\mingw64\lib"
RM_CMD = del /S *.o
endif
ifeq ($(shell uname), Linux)
$(info Operating system is Linux)
RM_CMD = rm *.o; rm shaders/DXIL/*.*; rm shaders/SPIRV/*.*
endif

SHADER_LIST = $(wildcard shaders/*.hlsl.*)

all : $(EXE_NAME)

$(EXE_NAME): $(OBJS) $(SRCS) $(DXIL_SHADERS) $(SPIRV_SHADERS)
	g++ -o $(EXE_NAME) $(OBJS) $(AOBJS) $(INC) $(LIB) $(WIN) -g -Wno-narrowing -lSDL3 -lSDL3_image -lSDL3_mixer -lglm -lSDL3_ttf -ljson-c -lcglm
	
$(OBJS):
	g++ $(INC) $(LIB) $(WIN) -g -Wno-narrowing -lSDL3 -lSDL3_image -lSDL3_mixer -lSDL3_ttf -lcglm -c $(SRCS_CPP)
	gcc $(INC) $(LIB) $(WIN) -g -Wno-narrowing -lSDL3 -lSDL3_image -lSDL3_mixer -lSDL3_ttf -lcglm -ljson-c -c $(SRCS_C)

$(DXIL_SHADERS):
	$(foreach cur_file, $(SHADER_LIST), shadercross $(cur_file) -s HLSL -d DXIL -o ./shaders/DXIL/$(notdir $(subst .hlsl,,$(cur_file)));)
	
$(SPIRV_SHADERS):
	$(foreach cur_file, $(SHADER_LIST), shadercross $(cur_file) -s HLSL -d SPIRV -o ./shaders/SPIRV/$(notdir $(subst .hlsl,,$(cur_file)));)

memleaks:
	valgrind --leak-check=full -s ./$(EXE_NAME)

clean:
	$(RM_CMD)
