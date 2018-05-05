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

# test acceptor 
TEST_ACCEPTOR_SRC=$(TEST_DIR)/TestAcceptor.cpp \
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
         
TEST_ACCEPTOR_OBJECTS+=$(addsuffix .o,$(basename $(TEST_ACCEPTOR_SRC)))

# test timefd 
TEST_TIMEFD_SRC=$(TEST_DIR)/TestReactor.cpp \
        $(SRC_DIR)/base/Timestamp.cpp \
        $(SRC_DIR)/net/Channel.cpp \
        $(SRC_DIR)/net/EventLoop.cpp \
    	$(SRC_DIR)/net/Poller.cpp \
		$(SRC_DIR)/net/Timer.cpp \
		$(SRC_DIR)/net/TimerQueue.cpp \
         
TEST_TIMEFD_OBJECTS+=$(addsuffix .o,$(basename $(TEST_TIMEFD_SRC)))

#test echo 
TEST_ECHO_SRC=$(TEST_DIR)/echo/main.cpp \
              $(TEST_DIR)/echo/echo.cpp \
        $(SRC_DIR)/base/Timestamp.cpp \
        $(SRC_DIR)/net/Channel.cpp \
	    $(SRC_DIR)/net/Buffer.cpp   \
        $(SRC_DIR)/net/Acceptor.cpp \
        $(SRC_DIR)/net/EventLoop.cpp \
	$(SRC_DIR)/net/InetAddress.cpp \
	$(SRC_DIR)/net/Poller.cpp \
	$(SRC_DIR)/net/Socket.cpp \
	$(SRC_DIR)/net/SocketsOps.cpp \
	$(SRC_DIR)/net/Timer.cpp \
	$(SRC_DIR)/net/TimerQueue.cpp \
	$(SRC_DIR)/net/TcpServer.cpp \
	$(SRC_DIR)/net/TcpConnection.cpp 
         
TEST_ECHO_OBJECTS+=$(addsuffix .o,$(basename $(TEST_ECHO_SRC))) 

#test timequeue 
TEST_TIMEQUEUE_SRC=$(TEST_DIR)/TestTimerQueue.cpp \
        $(SRC_DIR)/base/Timestamp.cpp \
        $(SRC_DIR)/base/TimerId.cpp \
        $(SRC_DIR)/net/Channel.cpp \
        $(SRC_DIR)/net/EventLoop.cpp \
	$(SRC_DIR)/net/Poller.cpp \
	$(SRC_DIR)/net/Socket.cpp \
	$(SRC_DIR)/net/SocketsOps.cpp \
	$(SRC_DIR)/net/Timer.cpp \
	$(SRC_DIR)/net/TimerQueue.cpp \
         
TEST_TIMEQUEUE_OBJECTS+=$(addsuffix .o,$(basename $(TEST_TIMEQUEUE_SRC))) 

#test http_server 
TEST_HTTPSERVER_SRC=$(TEST_DIR)/TestHttpServer.cpp \
        $(SRC_DIR)/base/Timestamp.cpp \
        $(SRC_DIR)/net/Channel.cpp \
	    $(SRC_DIR)/net/Buffer.cpp   \
        $(SRC_DIR)/net/Acceptor.cpp \
        $(SRC_DIR)/net/EventLoop.cpp \
	$(SRC_DIR)/net/InetAddress.cpp \
	$(SRC_DIR)/net/Poller.cpp \
	$(SRC_DIR)/net/Socket.cpp \
	$(SRC_DIR)/net/SocketsOps.cpp \
	$(SRC_DIR)/net/Timer.cpp \
	$(SRC_DIR)/net/TimerQueue.cpp \
	$(SRC_DIR)/net/TcpServer.cpp \
	$(SRC_DIR)/net/TcpConnection.cpp \
	$(SRC_DIR)/net/http/HttpContext.cpp \
	$(SRC_DIR)/net/http/HttpResponse.cpp \
	$(SRC_DIR)/net/http/HttpServer.cpp \

TEST_HTTPSERVER_OBJECTS+=$(addsuffix .o,$(basename $(TEST_HTTPSERVER_SRC))) 

all:$(TARGET) $(TEST_DIR)/test_accept $(TEST_DIR)/echo/test_echo \
    $(TEST_DIR)/test_timefd $(TEST_DIR)/test_timequeue $(TEST_DIR)/test_httpserver   

$(TARGET) : $(OBJECTS)
	@echo "Linking $@"
	$(CXX) $(OBJECTS) -o $@

$(TEST_DIR)/test_accept : $(TEST_ACCEPTOR_OBJECTS) 
	@echo "Linking $@"
	$(CXX) $(TEST_ACCEPTOR_OBJECTS) -o $@

$(TEST_DIR)/echo/test_echo : $(TEST_ECHO_OBJECTS) 
	@echo "Linking $@"
	$(CXX) $(TEST_ECHO_OBJECTS) -o $@

$(TEST_DIR)/test_timefd : $(TEST_TIMEFD_OBJECTS) 
	@echo "Linking $@"
	$(CXX) $(TEST_TIMEFD_OBJECTS) -o $@

$(TEST_DIR)/test_timequeue : $(TEST_TIMEQUEUE_OBJECTS) 
	@echo "Linking $@"
	$(CXX) $(TEST_TIMEQUEUE_OBJECTS)-o $@

$(TEST_DIR)/test_httpserver : $(TEST_HTTPSERVER_OBJECTS)
	@echo "Linking $@"
	$(CXX) $(TEST_HTTPSERVER_OBJECTS)-o $@
 
%.o:%.cpp
	@echo "compling $@"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	@echo "Cleaning saber"
	@rm -rvf $(OBJECTS) $(TARGET) 
	@echo "Cleaning test_accept"
	@rm -rvf $(TEST_ACCEPTOR_OBJECTS) $(TEST_DIR)/test_accept 

	@echo "Cleaning test_echo"
	@rm -rvf $(TEST_ECHO_OBJECTS) $(TEST_DIR)/echo/test_echo

	@echo "Cleaning test_timefd"
	@rm -rvf $(TEST_TIMEFD_OBJECTS) $(TEST_DIR)/test_timefd

	@echo "Cleaning test_timequeue"
	@rm -rvf $(TEST_TIMEQUEUE_OBJECTS) $(TEST_DIR)/test_timequeue

	@echo "Cleaning test_httpserver"
	@rm -rvf $(TEST_HTTPSERVER_OBJECTS) $(TEST_DIR)/test_httpserver





