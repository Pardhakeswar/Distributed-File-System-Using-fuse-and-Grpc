#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <bits/stdc++.h> 
using namespace std;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


void pairsort(double a[], std::string b[], int n) 
{ 
    pair<double, std::string> pairt[n]; 
  
    // Storing the respective array 
    // elements in pairs. 
    for (int i = 0; i < n; i++)  
    { 
        pairt[i].first = a[i]; 
        pairt[i].second = b[i]; 
    } 
  
    // Sorting the pair array. 
    sort(pairt, pairt + n); 
      
    // Modifying original arrays 
    for (int i = 0; i < n; i++)  
    { 
        a[i] = pairt[i].first; 
        b[i] = pairt[i].second; 
    } 
} 

int main()
{
// demo testing base logic
std::string inp_ip="152.46.16.179";
std::string someString = "ping -c 1 "+ inp_ip    +" | grep 'time' | awk '{print $7}' |awk '{ print substr( $0, 6, 4) }' | awk 'NR==1{print $1; exit}'";
std::string latency_str=exec(someString.c_str());
double temp = ::atof(latency_str.c_str());
// std::cout << temp << "\n";     



// actual logic starts here
int n_dataserver=2;
std::string ip_addresses[n_dataserver] = {"152.46.16.179","152.46.18.218"};
double latency_arr[n_dataserver]={0.0,0.0};

// get the latency for each ip addr
for (int i = 0; i < n_dataserver; i++) 
{
inp_ip=ip_addresses[i];
someString = "ping -c 1 "+ inp_ip    +" | grep 'time' | awk '{print $7}' |awk '{ print substr( $0, 6, 4) }' | awk 'NR==1{print $1; exit}'";
latency_str=exec(someString.c_str());
latency_arr[i] = ::atof(latency_str.c_str());
}

    std::cout << "before" << "";
    for (int i = 0; i < n_dataserver; i++)
       std::cout << latency_arr[i] << " ";
    std::cout << endl;

    for (int i = 0; i < n_dataserver; i++)
       std:: cout << ip_addresses[i] << " ";

      // sort the IPs based on latency array
      pairsort(latency_arr, ip_addresses, n_dataserver); 

    std::cout << "after" << "";
    for (int i = 0; i < n_dataserver; i++) 
       std::cout << latency_arr[i] << " "; 
    std::cout << endl; 
  
    for (int i = 0; i < n_dataserver; i++) 
       std:: cout << ip_addresses[i] << " "; 

return 0;
}
