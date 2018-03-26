TARGET=saber
CXX=g++
SRC_DIR=$(PWD)/src

#building flags
CXXFLAGS=-std=c++0x -g 

#source files
SOURCES=$(SRC_DIR)/main.cc \
        $(SRC_DIR)/base/Timestamp.cpp \
        $(SRC_DIR)/Channel.cpp \
	$(SRC_DIR)/EventLoop.cpp \
	$(SRC_DIR)/InetAddress.cpp \
	$(SRC_DIR)/Poller.cpp \
	$(SRC_DIR)/Socket.cpp \
	$(SRC_DIR)/SocketsOps.cpp \
	$(SRC_DIR)/Timer.cpp \
	$(SRC_DIR)/TimerQueue.cpp

OBJECTS+=$(addsuffix .o,$(basename $(SOURCES)))

all:$(TARGET)

$(TARGET) : $(OBJECTS)
	@echo "Linking $@"
	$(CXX) $< -o $@

%.o:%.cpp
	@echo "compling $@"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	@echo "Cleaning saber"
	@rm -rvf $(OBJECTS) $(TARGET) 




