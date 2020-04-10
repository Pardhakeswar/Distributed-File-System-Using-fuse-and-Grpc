#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <grpc++/grpc++.h>

#include "nfsfuse.grpc.pb.h"

#include "Nfsstaging.h"

#define READ_MAX    10000000

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using namespace nfsfuse;


using namespace std;

struct sdata{
	int a;
	char b[10]={};
};

// Class for 

NfsStaging nfs_data(grpc::CreateChannel(
			      "152.46.16.179:50052", grpc::InsecureChannelCredentials()));

NfsStaging nfs_data1(grpc::CreateChannel(
                              "localhost:50053", grpc::InsecureChannelCredentials()));

class NfsServiceImpl_staging final : public NFS_Stagingserver::Service {
	Status nfsfuse_getattr(ServerContext* context, const String* s, 
					 Stat* reply) override {
	(void) context;
	cout<<"Inisde  nfsfuse_getattr staging "<<endl;	
	Status status = nfs_data.rpc_getattr_data(*s, reply);
	Status status1 = nfs_data1.rpc_getattr_data(*s, reply);

        return status1 ;
	
	}
	
	/*	
	Status nfsfuse_readdir(ServerContext* context, const String* s) {
		//cout<<"[DEBUG] : readdir: "<<s->str().c_str()<<endl;
		(void) context;
		cout<<"Inisde  nfsfuse_readdir staging "<<endl;
		Status status = nfs_data.rpc_readdir_data(*s);
		return status;

	}
	*/	
    Status nfsfuse_open(ServerContext* context, const FuseFileInfo* fi_req,
            FuseFileInfo* fi_reply) override {
	(void) context;
        cout<<"Inisde  nfsfuse_open staging "<<endl;
        Status status = nfs_data.rpc_open_data(*fi_req, fi_reply);
	Status status1 = nfs_data1.rpc_open_data(*fi_req, fi_reply);
        return status1;
    }

    Status nfsfuse_read(ServerContext* context, const ReadRequest* rr, 
            ReadResult* reply) override {
        (void) context;
	cout<<"Inisde  nfsfuse_readstaging "<<endl;
        Status status = nfs_data.rpc_read_data(*rr, reply);	
	Status status1 = nfs_data1.rpc_read_data(*rr, reply);
        return status1;
    }


    Status nfsfuse_write(ServerContext* context, const WriteRequest* wr, 
            WriteResult* reply) override {
	(void) context;
	cout<<"Inisde  nfsfuse_write staging "<<endl; 
        Status status = nfs_data.rpc_write_data(*wr, reply);	
        Status status1 = nfs_data1.rpc_write_data(*wr, reply);	
        return status1;
    }


    Status nfsfuse_create(ServerContext* context, const CreateRequest* req,
            CreateResult* reply) override {
	    (void) context;	
	    cout<<"Inisde  nfsfuse_create staging "<<endl;
	    Status status = nfs_data.rpc_create_data(*req, reply);
	    Status status1 = nfs_data1.rpc_create_data(*req, reply);
            return status1;
    }


    Status nfsfuse_mkdir(ServerContext* context, const MkdirRequest* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : mkdir: " << endl;
	    (void) context;
	    cout<<"Inisde  nfsfuse_mkdir staging "<<endl;
	    Status status = nfs_data.rpc_mkdir_data(*input, reply);
	    Status status1 = nfs_data1.rpc_mkdir_data(*input, reply);
            return status1;
    }

    Status nfsfuse_rmdir(ServerContext* context, const String* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : rmdir: " << endl;
	    (void) context;
	    cout<<"Inisde  nfsfuse_rmdir staging "<<endl;
            Status status = nfs_data.rpc_rmdir_data(*input, reply);
	    Status status1 = nfs_data1.rpc_rmdir_data(*input, reply);

            return status1;
    }


    Status nfsfuse_unlink(ServerContext* context, const String* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : unlink " << endl;
	    
            (void) context;	    
	    cout<<"Inisde  nfsfuse_unlink staging "<<endl;
	    Status status = nfs_data.rpc_unlink_data(*input, reply);
	    Status status1 = nfs_data1.rpc_unlink_data(*input, reply);
            return status1;
    }

    Status nfsfuse_rename(ServerContext* context, const RenameRequest* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : rename " << endl;

	(void) context;    
        cout<<"Inisde  nfsfuse_rename staging "<<endl;
	Status status = nfs_data.rpc_rename_data(*input, reply);
        Status status1 = nfs_data1.rpc_rename_data(*input, reply);
        return status1;
    }

    Status nfsfuse_utimens(ServerContext* context, const UtimensRequest* input,
                                         OutputInfo* reply) override {
        //cout<<"[DEBUG] : utimens " << endl;
	(void) context;
	cout<<"Inisde  nfsfuse_utimens staging "<<endl;
        Status status = nfs_data.rpc_utimens_data(*input, reply);
	Status status1 = nfs_data1.rpc_utimens_data(*input, reply);	
        return status1;
    }

    Status nfsfuse_mknod(ServerContext* context, const MknodRequest* input,
                                         OutputInfo* reply) override {
       // cout<<"[DEBUG] : mknod " << endl;
        (void) context;
	cout<<"Inisde  nfsfuse_mknod staging "<<endl;
	Status status = nfs_data.rpc_mknod_data(*input, reply);
   	Status status1 = nfs_data1.rpc_mknod_data(*input, reply);
        return status1;
    }
	
};

void RunServer(string ip) {
  std:: string Ip_port = ip+":"+"50051";
  std::string server_address(Ip_port);
  NfsServiceImpl_staging service;

  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) {


  std::string ip = argv[1]; 		
  RunServer(ip);

  return 0;
}

