PROJECT := test

LIBS := pthread rt
INCLIDES :=
CPPFLAGS := -W -Wall -Werror -O2 -g
LDFLAGS :=

SOURCES := $(wildcard *.cpp)

OBJECTS := $(SOURCES:.cpp=.o)

all:$(PROJECT)

$(PROJECT):$(OBJECTS)
	g++ $(LDFLAGS) $^ -o $@ $(addprefix -l, $(LIBS))

%.o: %.cpp
	g++ $(CPPFLAGS) $(addprefix -I, $(INCLIDES)) -c $< -o $@

clean:
	rm -f $(PROJECT) $(OBJECTS)

run:$(PROJECT)
	./$(PROJECT)