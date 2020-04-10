#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <queue>
#include <vector>

#include <grpc++/grpc++.h>

#include "dfsfuse.grpc.pb.h"

#include "Dfsstaging.h"

#define READ_MAX    10000000

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using namespace dfsfuse;


using namespace std;

struct sdata{
	int a;
	char b[10]={};
};


vector<string> data_servers_ip{"152.46.17.6:50053", "152.46.16.179:50052", "152.46.18.175:50054"};

// Objects for data-servers 

//NfsStaging nfs_data = new NfsStaging(grpc::CreateChannel(
//			      data_servers_ip[0], grpc::InsecureChannelCredentials()));

//NfsStaging nfs_data1 = new NfsStaging(grpc::CreateChannel(
//                              data_servers_ip[1], grpc::InsecureChannelCredentials()));

//queue<NfsStaging> nfs_data_servers;

class NfsServiceImpl_staging final : public NFS_Stagingserver::Service {
    private:
        queue<NfsStaging*> nfs_data_servers;
	NfsStaging* nfs_data;
	NfsStaging* nfs_data1;
	NfsStaging* nfs_data2;

    public:
	//queue<NfsStaging> nfs_data_servers;
    NfsServiceImpl_staging() {
	nfs_data = new NfsStaging(grpc::CreateChannel(
				data_servers_ip[0], grpc::InsecureChannelCredentials()));
	nfs_data1 = new NfsStaging(grpc::CreateChannel(
				data_servers_ip[1], grpc::InsecureChannelCredentials()));
	nfs_data2 = new NfsStaging(grpc::CreateChannel(
                                data_servers_ip[2], grpc::InsecureChannelCredentials()));
	nfs_data_servers.push(nfs_data);
        nfs_data_servers.push(nfs_data1);
	nfs_data_servers.push(nfs_data2);
    }
    Status nfsfuse_getattr(ServerContext* context, const String* s, 
					 Stat* reply) override {
	(void) context;
	cout<<"[DEBUG]: dfsfuse_getattr function called in staging server"<<endl;
	int loop = 1;
	int fail_conns = 0;
	int num_conns = 0;

	while(loop) {
		NfsStaging* nfs_stag = nfs_data_servers.front();

        	nfs_data_servers.pop();
		Status status = nfs_stag->rpc_getattr_data(*s, reply);
		nfs_data_servers.push(nfs_stag);

		if(status.ok()) {
			cout << "[DEBUG]: Getattr file successful for data server "<< num_conns<<endl;
        	        return status;
		}
		fail_conns++;
		num_conns++;
		if(fail_conns == 2*nfs_data_servers.size()) {
			cout << "[ERROR] " << nfs_data_servers.size()<<" data servers are not reachable at the moment";
			loop = 0;
			return status;
		}

	}
	
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
	int num_conns = 0;
	int success = 0;
	Status stat;
	Status status;

        cout<<"[DEBUG]: dfsfuse_open function called in staging server"<<endl;

	while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();

                nfs_data_servers.pop();
                status = nfs_stag->rpc_open_data(*fi_req, fi_reply);
                nfs_data_servers.push(nfs_stag);

                if (status.ok()) {
			cout << "[DEBUG]: Open file successful for data server "<< num_conns<<endl;
			success = 1;
			stat = status;
                } else {
			cout << "[ERROR]: Open file failed for data server "<< num_conns<<endl;
		}
		num_conns++;

        }
	if (success)
		return stat;
	else 
		return status;
    }

    Status nfsfuse_read(ServerContext* context, const ReadRequest* rr, 
            ReadResult* reply) override {
        (void) context;
	cout<<"[DEBUG]: dfsfuse_read function called in staging server "<<endl;

	int loop = 1;
        int fail_conns = 0;
	int num_conns = 0;

        while(loop) {
                NfsStaging* nfs_stag = nfs_data_servers.front();

                nfs_data_servers.pop();
                Status status = nfs_stag->rpc_read_data(*rr, reply);
                nfs_data_servers.push(nfs_stag);
		num_conns++;

                if(status.ok()) {
			cout << "[DEBUG]: Read file successful for data server "<< num_conns<<endl;
                        return status;
                }
                fail_conns++;
                if(fail_conns == 2*nfs_data_servers.size()) {
                        cout << "[ERROR] " << nfs_data_servers.size()<<" data servers are not reachable at the moment";
                        loop = 0;
                        return status;
                }

        }

    }


    Status nfsfuse_write(ServerContext* context, const WriteRequest* wr, 
            WriteResult* reply) override {
	(void) context;
	cout<<"[DEBUG]: nfsfuse_write function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;

        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();

                nfs_data_servers.pop();
                status = nfs_stag->rpc_write_data(*wr, reply);
                nfs_data_servers.push(nfs_stag);

                if (status.ok()) {
                        cout << "[DEBUG]: Write file successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: Write file failed for data server "<< num_conns<<endl;
                }
                num_conns++;

        }
        if (success)
                return stat;
        else
                return status;

    }


    Status nfsfuse_create(ServerContext* context, const CreateRequest* req,
            CreateResult* reply) override {
	(void) context;
	cout<<"[DEBUG]: nfsfuse_create function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;
        
        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();
                
                nfs_data_servers.pop();
                status = nfs_stag->rpc_create_data(*req, reply);
                nfs_data_servers.push(nfs_stag);
                
                if (status.ok()) {
                        cout << "[DEBUG]: Create file successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: Create file failed for data server "<< num_conns<<endl;
                }
                num_conns++;
        
        }
        if (success)
                return stat;
        else    
                return status;

    }


    Status nfsfuse_mkdir(ServerContext* context, const MkdirRequest* input,
                                         OutputInfo* reply) override {
	(void) context;
	cout<<"[DEBUG]: nfsfuse_mkdir function called in staging server "<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;
        
        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();
                
                nfs_data_servers.pop();
                status = nfs_stag->rpc_mkdir_data(*input, reply);
                nfs_data_servers.push(nfs_stag);
                
                if (status.ok()) {
                        cout << "[DEBUG]: Make Dir successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: Make Dir failed for data server "<< num_conns<<endl;
                }
                num_conns++;
        
        }
        if (success)
                return stat;
        else    
                return status;
        
    }

    Status nfsfuse_rmdir(ServerContext* context, const String* input,
                                         OutputInfo* reply) override {
	(void) context;
	cout<<"[DEBUG]: nfsfuse_rmdir function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;

        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();

                nfs_data_servers.pop();
                status = nfs_stag->rpc_rmdir_data(*input, reply);
                nfs_data_servers.push(nfs_stag);

                if (status.ok()) {
                        cout << "[DEBUG]: Remove Dir successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: Remove Dir failed for data server "<< num_conns<<endl;
                }
                num_conns++;

        }
        if (success)
                return stat;
        else
                return status;
    }


    Status nfsfuse_unlink(ServerContext* context, const String* input,
                                         OutputInfo* reply) override {
	    
	(void) context;	    
	cout<<"[DEBUG]: nfsfuse_unlink function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;
        
        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();
                
                nfs_data_servers.pop();
                status = nfs_stag->rpc_unlink_data(*input, reply);
                nfs_data_servers.push(nfs_stag);
                
                if (status.ok()) {
                        cout << "[DEBUG]: Unlink successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: Unlink failed for data server "<< num_conns<<endl;
                }
                num_conns++;
        
        }
        if (success)
                return stat;
        else    
                return status;
    }

    Status nfsfuse_rename(ServerContext* context, const RenameRequest* input,
                                         OutputInfo* reply) override {

	(void) context;    
        cout<<"[DEBUG]: nfsfuse_rename function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;
        
        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();
                
                nfs_data_servers.pop();
                status = nfs_stag->rpc_rename_data(*input, reply);
                nfs_data_servers.push(nfs_stag);
                
                if (status.ok()) {
                        cout << "[DEBUG]: File rename successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: File rename for data server "<< num_conns<<endl;
                }
                num_conns++;
        
        }
        if (success)
                return stat;
        else    
                return status;
    }

    Status nfsfuse_utimens(ServerContext* context, const UtimensRequest* input,
                                         OutputInfo* reply) override {

	(void) context;
	cout<<"[DEBUG]: nfsfuse_utimens function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;
        
        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();
                
                nfs_data_servers.pop();
                status = nfs_stag->rpc_utimens_data(*input, reply);
                nfs_data_servers.push(nfs_stag);
                
                if (status.ok()) {
                        cout << "[DEBUG]: File utimens successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: File utimens for data server "<< num_conns<<endl;
                }
                num_conns++;
        
        }
        if (success)
                return stat;
        else    
                return status;
    }

    Status nfsfuse_mknod(ServerContext* context, const MknodRequest* input,
                                         OutputInfo* reply) override {
        (void) context;
	cout<<"[DEBUG]: nfsfuse_mknod function called in staging server"<<endl;

	int num_conns = 0;
        int success = 0;
        Status stat;
	Status status;
        
        while(num_conns < nfs_data_servers.size()) {
                NfsStaging* nfs_stag = nfs_data_servers.front();
                
                nfs_data_servers.pop();
                status = nfs_stag->rpc_mknod_data(*input, reply);
                nfs_data_servers.push(nfs_stag);
                
                if (status.ok()) {
                        cout << "[DEBUG]: File make node successful for data server "<< num_conns<<endl;
                        success = 1;
                        stat = status;
                } else {
                        cout << "[ERROR]: File make node for data server "<< num_conns<<endl;
                }
                num_conns++;
        
        }
        if (success)
                return stat;
        else    
                return status;
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

