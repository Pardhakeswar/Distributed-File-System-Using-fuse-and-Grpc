#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <grpc++/grpc++.h>

#include "dfsfuse.grpc.pb.h"

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

void translatePath(const char* client_path, char* server_path){
    memset(server_path, '\0', strlen(server_path));
    strcat(server_path, "./dataserver");
    strcat(server_path+8, client_path);
    server_path[strlen(server_path)] = '\0';
}

class NfsServiceImpl_data final : public NFS_Dataserver::Service {
	Status nfsfuse_getattr_data(ServerContext* context, const String* s, 
					 Stat* reply) override {
		//cout<<"[DEBUG] : lstat: "<<s->str().c_str()<<endl;

		cout<<"[DEBUG:] dfsfuse_getattr_data function called in data server"<<endl;
		struct stat st;
		char server_path[512]={0};
		translatePath(s->str().c_str(), server_path);
		int res = lstat(server_path, &st);
        if(res == -1){
		    perror(strerror(errno));
            //cout<<"errno: "<<errno<<endl;
		    reply->set_err(errno);
		}
		else{
            reply->set_ino(st.st_ino);
            reply->set_mode(st.st_mode);
            reply->set_nlink(st.st_nlink);
            reply->set_uid(st.st_uid);
            reply->set_gid(st.st_gid);

            reply->set_size(st.st_size);
            reply->set_blksize(st.st_blksize);
            reply->set_blocks(st.st_blocks);
            reply->set_atime(st.st_atime);
            reply->set_mtime(st.st_mtime);
            reply->set_ctime(st.st_ctime);
			
		    reply->set_err(0);
		}
		
        return Status::OK;
	
	}
	/*	
	Status nfsfuse_readdir_data(ServerContext* context, const String* s,
						ServerWriter<Dirent>* writer) override {
		//cout<<"[DEBUG] : readdir: "<<s->str().c_str()<<endl;

		DIR *dp;
		struct dirent *de;
		Dirent directory;
		char server_path[512]={0};
		translatePath(s->str().c_str(), server_path);

		dp = opendir(server_path);
		if (dp == NULL){
			//cout<<"[DEBUG] : readdir: "<<"dp == NULL"<<endl;
			perror(strerror(errno));
			directory.set_err(errno);
            return Status::OK;
		}
			
		while((de = readdir(dp)) != NULL){
		    directory.set_dino(de->d_ino);
		    directory.set_dname(de->d_name);
		    directory.set_dtype(de->d_type);
		    writer->Write(directory);
		}
		directory.set_err(0);

		closedir(dp);

		return Status::OK;
	}
	*/
    Status nfsfuse_open_data(ServerContext* context, const FuseFileInfo* fi_req,
            FuseFileInfo* fi_reply) override {
        
        char server_path[512] = {0};
        cout<<"[DEBUG:] dfsfuse_open_data function called in data server"<<endl;
        translatePath(fi_req->path().c_str(), server_path);
		//cout<<"[DEBUG] : nfsfuse_open: path "<<server_path<<endl;
		//cout<<"[DEBUG] : nfsfuse_open: flag "<<fi_req->flags()<<endl;

        int fh = open(server_path, fi_req->flags());

		//cout<<"[DEBUG] : nfsfuse_open: fh"<<fh<<endl;
        if(fh == -1){
            fi_reply->set_err(errno);            
        }
        else{
            fi_reply->set_fh(fh);            
            fi_reply->set_err(0);
            close(fh);
        }
        
        return Status::OK;
    }

    Status nfsfuse_read_data(ServerContext* context, const ReadRequest* rr, 
            ReadResult* reply) override {
        char path[512];
        char *buf = new char[rr->size()];
	cout<<"[DEBUG:] dfsfuse_read_data in function called in data server"<<endl;
        translatePath(rr->path().c_str(), path);
		//cout<<"[DEBUG] : nfsfuse_read: "<<path<<endl;


        int fd = open(path, O_RDONLY);
		//cout<<"[DEBUG] : nfsfuse_read: fd "<<fd<<endl;
        if (fd == -1){
            reply->set_err(errno);
		    perror(strerror(errno));
            return Status::OK;
        }

        int res = pread(fd, buf, rr->size(), rr->offset());
        if (res == -1){
            reply->set_err(errno);
		    perror(strerror(errno));
            return Status::OK;
        }

        reply->set_bytesread(res);
        reply->set_buffer(buf);
        reply->set_err(0);
        
        if(fd>0)
            close(fd);
        free(buf);
        
        return Status::OK;
    }


    Status nfsfuse_write_data(ServerContext* context, const WriteRequest* wr, 
            WriteResult* reply) override {
        char path[512] = {0};
        translatePath(wr->path().c_str(), path);
	cout<<" [DEBUG:] dfsfuse_write_data function called in data server"<<endl;
        int fd = open(path, O_WRONLY);
		//cout<<"[DEBUG] : nfsfuse_write: path "<<path<<endl;
		//cout<<"[DEBUG] : nfsfuse_write: fd "<<fd<<endl;
        if(fd == -1){
            reply->set_err(errno);
            perror(strerror(errno));
            return Status::OK;
        } 

        int res = pwrite(fd, wr->buffer().c_str(), wr->size(), wr->offset());
		//cout<<"[DEBUG] : nfsfuse_write: res"<<res<<endl;

        fsync(fd);

        if(res == -1){
            reply->set_err(errno);
            perror(strerror(errno));
            return Status::OK;
        }
        
        reply->set_nbytes(res);
        reply->set_err(0);
    
        if(fd>0)
            close(fd);

        return Status::OK;
    }


    Status nfsfuse_create_data(ServerContext* context, const CreateRequest* req,
            CreateResult* reply) override {

        char server_path[512] = {0};
	cout<<"[DEBUG:] dfsfuse_create_data function called in data server"<<endl;
        translatePath(req->path().c_str(), server_path);

        //cout<<"[DEBUG] : nfsfuse_create: path "<<server_path<<endl;
        //cout<<"[DEBUG] : nfsfuse_create: flag "<<req->flags()<<endl;

        int fh = open(server_path, req->flags(), req->mode());

        //cout<<"[DEBUG] : nfsfuse_create: fh"<<fh<<endl;
        if(fh == -1){
            reply->set_err(errno);
            return Status::OK;
        }
        else{
            reply->set_fh(fh);
            reply->set_err(0);
            close(fh);
            return Status::OK;
        }
    }


    Status nfsfuse_mkdir_data(ServerContext* context, const MkdirRequest* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : mkdir: " << endl;
		
            char server_path[512]={0};
	    cout<<"[DEBUG:] dfsfuse_mkdir_data function called in data server"<<endl;
            translatePath(input->s().c_str(), server_path);

            int res = mkdir(server_path, input->mode());
		

            if (res == -1) {
                perror(strerror(errno)); 
                reply->set_err(errno);
                return Status::OK;
            } else {
	            reply->set_err(0);
            }

            return Status::OK;
    }

    Status nfsfuse_rmdir_data(ServerContext* context, const String* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : rmdir: " << endl;
	    cout<<"[DEBUG:] dfsfuse_rmdir_data function called in data server"<<endl;
            char server_path[512]={0};
            translatePath(input->str().c_str(), server_path);

            int res = rmdir(server_path);


            if (res == -1) {
                perror(strerror(errno));
                reply->set_err(errno);
                return Status::OK;
            } else {
                reply->set_err(0);
            }
            
            return Status::OK;
    }


    Status nfsfuse_unlink_data(ServerContext* context, const String* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : unlink " << endl;
	    cout<<"[DEBUG:] dfsfuse_unlink_data function called in data server"<<endl;
            char server_path[512]={0};
            translatePath(input->str().c_str(), server_path);
           // cout << "server path: " << server_path << endl;
            int res = unlink(server_path);
            if (res == -1) {
                perror(strerror(errno));
                reply->set_err(errno);
                return Status::OK;
            } else {
                reply->set_err(0);
            }
            return Status::OK;
    }

    Status nfsfuse_rename_data(ServerContext* context, const RenameRequest* input,
                                         OutputInfo* reply) override {
            //cout<<"[DEBUG] : rename " << endl;
	    cout<<"[DEBUG:] dfsfuse_rename_data function called in data server"<<endl;
	    if (input->flag()) {
            perror(strerror(errno));
            reply->set_err(EINVAL);
            reply->set_str("rename fail");	        
	        return Status::OK;
	    }

        char from_path[512]={0};
        char to_path[512] = {0};
        translatePath(input->fp().c_str(), from_path);
 	    translatePath(input->tp().c_str(), to_path);


	    int res = rename(from_path, to_path);
        if (res == -1) {
            perror(strerror(errno));
            reply->set_err(errno);
            return Status::OK;
        } else {
            reply->set_err(0);
        }
        
        return Status::OK;
    }

    Status nfsfuse_utimens_data(ServerContext* context, const UtimensRequest* input,
                                         OutputInfo* reply) override {
        //cout<<"[DEBUG] : utimens " << endl;

	char server_path[512]={0};
	cout<<"[DEBUG:] dfsfuse_utimens_data function called in data server"<<endl;
        translatePath(input->path().c_str(), server_path);


        struct timespec ts[2];
	long oo;
	int ii;

	ts[0].tv_sec = input->sec();
	ts[0].tv_nsec = input->nsec();

	ts[1].tv_sec = input->sec2();
	ts[1].tv_nsec = input->nsec2();

        int res = utimensat(AT_FDCWD, server_path, ts, AT_SYMLINK_NOFOLLOW);


	if (res == -1) {
            perror(strerror(errno));
            reply->set_err(errno);
            return Status::OK;
        } 
        reply->set_err(0);
        return Status::OK;
    }

    Status nfsfuse_mknod_data(ServerContext* context, const MknodRequest* input,
                                         OutputInfo* reply) override {
       // cout<<"[DEBUG] : mknod " << endl;

        char server_path[512]={0};
        translatePath(input->path().c_str(), server_path);
	  cout<<"[DEBUG:] dfsfuse_mknod_data function called in data server"<<endl;
	    mode_t mode = input->mode();
	    dev_t rdev = input->rdev();

	    int res;

	    if (S_ISFIFO(mode))
		    res = mkfifo(server_path, mode);
	    else
		    res = mknod(server_path, mode, rdev);
	    
        if (res == -1) {
	        reply->set_err(errno);
            return Status::OK;
	    }

        reply->set_err(0);
        return Status::OK;
    }

};

void RunServer(string ip) {
  std:: string Ip_port = ip+":"+"50053";
  std::string server_address(Ip_port);
  NfsServiceImpl_data service;

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

