#################CSC 568 Distributed File system##############


make clean
make
####Running the server 
mkdir ./server
./nfsfuse_server <Ip adress of server>


### Running the client    
mkdir ./client        
./nfsfuse_client <Ip adress of server> ./client
