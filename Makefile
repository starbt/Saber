TARGET=http_server
CXX=g++
PWD=$(shell pwd)
SRC_DIR=$(PWD)/src
HDR_DIR = $(SRC_DIR) $(PWD)/include

#building flags
CXXFLAGS=-std=c++0x -g

#source files
SOURCES=$(SRC_DIR)/main.cc \
        $(SRC_DIR)/core/fd_channel.cc \
        $(SRC_DIR)/core/epoller.cc \
        $(SRC_DIR)/core/event_loop.cc \
        $(SRC_DIR)/core/event_loop_thread.cc \
        $(SRC_DIR)/core/event_loop_thread_pool.cc \
        $(SRC_DIR)/time/timer_stamp.cc \
        $(SRC_DIR)/time/timer_manager.cc \
        $(SRC_DIR)/time/timer.cc \
        $(SRC_DIR)/net/tcp_acceptor.cc \
        $(SRC_DIR)/net/tcp_connection.cc \
        $(SRC_DIR)/net/tcp_server.cc \
        $(SRC_DIR)/net/buffer.cc \
        $(SRC_DIR)/net/inet_address.cc \
        $(SRC_DIR)/net/socket.cc \
        $(SRC_DIR)/net/sockets_util.cc \
        $(SRC_DIR)/manage/config_loader.cc \
        $(SRC_DIR)/manage/signal_manager.cc \
        $(SRC_DIR)/manage/pid_file_manager.cc \
        $(SRC_DIR)/http/http_context.cc \
        $(SRC_DIR)/http/http_response.cc \
        $(SRC_DIR)/http/http_server.cc 

HDR_INCLUDE = $(addprefix -I, $(HDR_DIR))
OBJECTS+=$(addsuffix .o,$(basename $(SOURCES)))

$(warning $(HDR_INCLUDE))

all:$(TARGET) 

$(TARGET):$(OBJECTS)
	@echo "Linking $@"
	$(CXX) $(OBJECTS) -lpthread -o $@

%.o:%.cc
	@echo "compling $@"
	@$(CXX) -c $(HDR_INCLUDE) $(CXXFLAGS) $< -o $@

clean:
	@echo "Cleaning http_server"
	@rm -rvf $(OBJECTS) $(TARGET) 



