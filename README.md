###### CSC-568Project

ESA Status Report 1 - Distributed FS using FUSE

**Project Description:**

Implementation of a DFS using a FUSE based backend, that provides replication, location transparency among other features

**Team Members: Gautam Worah (gworah), Pardhakeswar Pacha (ppacha), Tapas Mallick (trmallic) **

**Current Status: **

We are able to create a server and client that interact via gRPC. The client send the commands to the server via gRPC and the server executed the command locally using FUSE. These results are returned to the client. We are able to successfully create files, directories, and edit them via the client and the corresponding changes are visible in the server as well. In the next phase we will focus on implementing deduplication in the server. 

**Steps to build the system: **

https://github.com/libfuse/libfuse install libfuse from this link

https://github.com/libfuse/libfuse/issues/178 -- move file to init.d folder did this next

https://github.com/libfuse/libfuse/issues/212 sudo ln -s /usr/local/lib/x86_64-linux-gnu/libfuse3.so.3.4.1 /lib/x86_64-linux-gnu/libfuse3.so.3

**Building the project **

./make clean

./make

**Running the server **

mkdir ./server

./nfsfuse_server

**Running the client **

mkdir ./client

./nfsfuse_client ./client

**GitHub repository: https://github.ncsu.edu/gworah/CSC-568Project**


