#######Replication Enabled ##########

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


Currently ports are Hardcoded needed to change them.
