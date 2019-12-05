CXXFLAGS+= -I . -I /usr/local/neuware/include -std=c++11 -g
LDFLAGS+= -L /usr/local/neuware/lib64  -Wl,-rpath=/usr/local/neuware/lib64 -lcnrt  -lcnml -lpthread -lopenblas

CPP_SRCS=$(wildcard *.cpp)
CPP_OBJS=$(CPP_SRCS:%.cpp=%.o)

MLU_SRCS=$(wildcard *.mlu)
MLU_OBJS=$(MLU_SRCS:%.mlu=%.o)

OBJS=$(CPP_OBJS) $(MLU_OBJS)

TARGET=test

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CXX) -O3 -o $@ -g $(OBJS) $(LDFLAGS)

%.o : %.cpp
	g++ $(CXXFLAGS) -c $^ -o $@

%.o : %.mlu
	cncc -c $^ -o $@ 

clean:
	rm -f $(TARGET) $(OBJS)
