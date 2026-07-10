CXX = c++

ifeq ($(WARNINGS), 2)
	CXXFLAGS = -Wall -Wextra -std=c++20 -MMD
else
	CXXFLAGS = -w -std=c++17 -MMD
endif


INC = -I./inc -I./Smp -I.

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)


.PHONY: all clean

all: build/scheduler

build/scheduler: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC) $(WARN) $(OBJS) -o $@

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INC) $(WARN) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf build
