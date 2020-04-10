#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include "NfsClient.h"

static struct options {	
	NfsClient* nfsclient;
	int show_help;
} options;

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

static void show_help(const char *progname)
{
	std::cout<<"usage: "<<progname<<" [-s -d] <mountpoint>\n\n";
}

static void *client_init(struct fuse_conn_info *conn, 
				struct fuse_config *cfg){
    (void) conn;
    return NULL;
}

static int client_getattr(const char *path, struct stat *stbuf,
             struct fuse_file_info *fi)
{
    memset(stbuf, 0, sizeof(struct stat));		
	return options.nfsclient->rpc_getattr(path, stbuf);
}

static int client_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
               off_t offset, struct fuse_file_info *fi,
               enum fuse_readdir_flags flags)
{
    return options.nfsclient->rpc_readdir(path, buf, filler);
}

static int client_open(const char *path, struct fuse_file_info *fi)
{
    return options.nfsclient->rpc_open(path, fi);
}

static int client_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi)
{
    return options.nfsclient->rpc_read(path, buf, size, offset, fi);
}

static int client_write(const char *path, const char *buf, size_t size,
             off_t offset, struct fuse_file_info *fi)
{
    return options.nfsclient->rpc_write(path, buf, size, offset, fi);
}

static int client_mkdir(const char *path, mode_t mode) {
    return options.nfsclient->rpc_mkdir(path, mode);
}

static int client_rmdir(const char *path)
{
    return options.nfsclient->rpc_rmdir(path);
}


static int client_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    return options.nfsclient->rpc_create(path, mode, fi);
}

static int client_unlink(const char *path)
{
    return options.nfsclient->rpc_unlink(path);    
}

static int client_rename(const char *from, const char *to, unsigned int flags)
{
    return options.nfsclient->rpc_rename(from, to, flags);
}


static int client_utimens(const char *path, const struct timespec ts[2],
		       struct fuse_file_info *fi)
{

    return options.nfsclient->rpc_utimens(path, ts, fi);
}

static int client_mknod(const char *path, mode_t mode, dev_t rdev)
{
    return options.nfsclient->rpc_mknod(path, mode, rdev);
}

static struct client_operations : fuse_operations {
    client_operations(){
        init = client_init;
        getattr = client_getattr;
        readdir = client_readdir;
        open = client_open;
        read = client_read;
        write = client_write;
        create  = client_create;
	mkdir	= client_mkdir;
	rmdir = client_rmdir;
	unlink  = client_unlink;
	rename = client_rename;
	utimens = client_utimens;
	mknod = client_mknod;

    }

} client_oper;



int main(int argc, char* argv[]){

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	options.nfsclient = new NfsClient(grpc::CreateChannel(
  "0.0.0.0:50051", grpc::InsecureChannelCredentials()));


    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
	return 1;

    if (options.show_help) {
        show_help(argv[0]);
        assert(fuse_opt_add_arg(&args, "--help") == 0);
        args.argv[0] = (char*) "";
    }


    return fuse_main(argc, argv, &client_oper, &options);
}
















