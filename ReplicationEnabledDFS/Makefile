CXX = g++
CPPFLAGS += -I/usr/local/include -g -pthread `pkg-config fuse3 --cflags --libs`
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib -g `pkg-config grpc++ grpc fuse3 --cflags --libs`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`


PROTOS_PATH = ./

vpath %.proto $(PROTOS_PATH)

all: system-check dfsfuse_client dfsfuse_staging dfsfuse_staging_replication dfsfuse_dataserver batch_write

dfsfuse_client: dfsfuse.pb.o dfsfuse.grpc.pb.o dfsfuse_client.o 
	$(CXX) $^ $(LDFLAGS) -o $@

dfsfuse_staging: dfsfuse.pb.o dfsfuse.grpc.pb.o dfsfuse_staging.o
	$(CXX) $^ $(LDFLAGS) -o $@

dfsfuse_staging_replication: dfsfuse.pb.o dfsfuse.grpc.pb.o dfsfuse_staging_replication.o
	$(CXX) $^ $(LDFLAGS) -o $@

dfsfuse_dataserver: dfsfuse.pb.o dfsfuse.grpc.pb.o dfsfuse_dataserver.o
	$(CXX) $^ $(LDFLAGS) -o $@

batch_write: batch_write.cc
	$(CXX) batch_write.cc -o $@

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h dfsfuse_client dfsfuse_staging dfsfuse_staging_replication dfsfuse_dataserver batch_write


# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif
