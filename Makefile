TARGET=saber
CXX=g++
SRC_DIR=$(PWD)/src
TEST_DIR=$(PWD)/test

#building flags
CXXFLAGS=-std=c++0x -g 

#source files
SOURCES=$(SRC_DIR)/main.cc \
        $(SRC_DIR)/base/Timestamp.cpp \
        $(SRC_DIR)/net/Channel.cpp \
        $(SRC_DIR)/net/Acceptor.cpp \
        $(SRC_DIR)/net/EventLoop.cpp \
	$(SRC_DIR)/net/InetAddress.cpp \
	$(SRC_DIR)/net/Poller.cpp \
	$(SRC_DIR)/net/Socket.cpp \
	$(SRC_DIR)/net/SocketsOps.cpp \
	$(SRC_DIR)/net/Timer.cpp \
	$(SRC_DIR)/net/TimerQueue.cpp

OBJECTS+=$(addsuffix .o,$(basename $(SOURCES)))

# test files
TEST_ACCEPTOR_SRC=$(TEST_DIR)/TestAcceptor.cpp \
        $(SRC_DIR)/net/EventLoop.cpp \
	    $(SRC_DIR)/net/InetAddress.cpp \
         
TEST_ACCEPTOR_OBJECTS+=$(addsuffix .o,$(basename $(TEST_ACCEPTOR_SRC)))

all:$(TARGET) $(TEST_DIR)/test_accept 

$(TARGET) : $(OBJECTS)
	@echo "Linking $@"
	$(CXX) $< -o $@

$(TEST_DIR)/test_accept:$(TEST_ACCEPTOR_OBJECTS) 
	@echo "Linking $@"
	@$(CXX) $< -o $@

%.o:%.cpp
	@echo "compling $@"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	@echo "Cleaning saber"
	@rm -rvf $(OBJECTS) $(TARGET) 
	@echo "Cleaning test_accept"
	@rm -rvf $(TEST_ACCEPTOR_OBJECTS) $(TEST_DIR)/test_accept



