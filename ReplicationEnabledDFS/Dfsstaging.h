#include <grpc++/grpc++.h>
#include "dfsfuse.grpc.pb.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;

using namespace dfsfuse;

using namespace std;


class NfsStaging {
 public:
  NfsStaging() {}
  NfsStaging(std::shared_ptr<Channel> channel)
      : stub_(NFS_Dataserver::NewStub(channel)) {}

    Status rpc_getattr_data(const String s, Stat* reply) {
        
	ClientContext context;
	
	std::cout<<"Inside rpc_getattr_data "<<std::endl;
        Status status = stub_->nfsfuse_getattr_data(&context, s, reply);
        

        return status;
    }
	/*
    
    Status rpc_readdir_data(const String s) {
        
	Status status;
        ClientContext ctx;

	std::cout<<"Inside rpc_readdir_data "<<std::endl;
        status = stub_->nfsfuse_readdir_data(&ctx, s);

        return status; 
    }
  */  
    Status rpc_open_data(const FuseFileInfo fi_req, FuseFileInfo* fi_reply ){
        ClientContext ctx;
        Status status; 
	std::cout<<"Inside rpc_open_data "<<std::endl;
        status = stub_->nfsfuse_open_data(&ctx, fi_req, fi_reply);

        return status;
    }
	
    Status rpc_read_data(const ReadRequest rr, ReadResult* reply) {
        ClientContext clientContext;
	std::cout<<"Inside rpc_read_data "<<std::endl;
        Status status = stub_->nfsfuse_read_data(&clientContext, rr, reply);

	return status;
    }

    Status rpc_write_data(const WriteRequest wr, WriteResult* reply){
        ClientContext ctx;
	
	std::cout<<"Inside rpc_write_data "<<std::endl;
        Status status = stub_->nfsfuse_write_data(&ctx, wr, reply);

	return status;
    }
    
    Status rpc_create_data(const CreateRequest req, CreateResult* reply) {
        ClientContext ctx; 
	std::cout<<"Inside rpc_create_data "<<std::endl;
        
        Status status = stub_->nfsfuse_create_data(&ctx, req, reply);
 
        return status;
    }

   Status rpc_mkdir_data(const MkdirRequest input, OutputInfo* reply)  {
      
      ClientContext context;
      std::cout<<"Inside rpc_create_data "<<std::endl;

      Status status = stub_->nfsfuse_mkdir_data(&context, input, reply);

      return status;
  }

  Status rpc_rmdir_data(const String input, OutputInfo* reply)  {
      ClientContext context;

      std::cout<<"Inside rpc_rmdir_data "<<std::endl;
      Status status = stub_->nfsfuse_rmdir_data(&context, input, reply);

      return status;
  }

	
  Status rpc_unlink_data(const String input, OutputInfo* reply)  {
      ClientContext context;

      std::cout<<"Inside rpc_unlink_data "<<std::endl;
      Status status = stub_->nfsfuse_unlink_data(&context, input, reply);

      return status;
  }

  Status rpc_rename_data(const RenameRequest input, OutputInfo* reply)  {
      ClientContext context;
      std::cout<<"Inside rpc_rename_data "<<std::endl;
      Status status = stub_->nfsfuse_rename_data(&context, input, reply);

      return status;
  }

  Status rpc_utimens_data(const UtimensRequest input, OutputInfo* reply)  {
      ClientContext context;
      std::cout<<"Inside rpc_rename_data "<<std::endl;
      Status status = stub_->nfsfuse_utimens_data(&context, input, reply);

      return status;
  }


  Status rpc_mknod_data(const MknodRequest input, OutputInfo* reply) {
      ClientContext context;
      std::cout<<"Inside rpc_mknod_data "<<std::endl;
      Status status = stub_->nfsfuse_mknod_data(&context, input, reply);
      return status;

  }



 private:
    std::unique_ptr<NFS_Dataserver::Stub> stub_;
};

