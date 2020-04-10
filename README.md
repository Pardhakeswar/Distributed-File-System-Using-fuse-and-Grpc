### CSC-568Project

 Distributed FS using FUSE and gRpc

Project Description:

Implementation of a Distributed based file system using a FUSE based backend and gRpc as the communication protocol, that provides replication, location transparency among other features

### Team Members 
Gautam Worah (gworah). \
Pardhakeswar Pacha (ppacha). \
Tapas Mallick (trmallic). 

For the complete Architecure and details look at the Presentation file. 

The 3 stage architecture has been implemented in the ReplicationEnabledDFS Folder.

### Fuse and gRpc Installation steps
Look at the file: fuse_and_grpc_install_steps

### Steps to run code.

use the ReplicationEnabled Folder

Run everything as root

make clean
make
### Running The data server ###########
mkdir ./dataserver. \
./nfsfuse_dataserver 0.0.0.0
### Running the staging server
./nfsfuse_staging 0.0.0.0
### Running the client
mkdir ./client
./nfsfuse_client 0.0.0.0 ./client      <<< on local machine


For any queries, contact ppacha@ncsu.edu 
