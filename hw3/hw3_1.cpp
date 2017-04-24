#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <sstream>
#include <math.h>
#include <iostream>
#include <msgpack.hpp>

struct dinfo{
	std::string name;
	std::string phone;
};

bool dsort(const struct dinfo &a, const struct dinfo &b) {
	return (a.phone < b.phone);
}

int main() {

    char *buf = NULL;
    size_t off = 0;
    int bufsize;
    msgpack::object_handle oh;
    std::vector<std::string> in_line;
    std::vector<std::string> out_line;

    std::fstream inf;
   // inf.open("input_example.txt", std::ios::in | std::ios::binary ); 
    inf.open("input.txt", std::ios::in | std::ios::binary ); 
    //To get the file size or you can use POSIX: stat 
    inf.seekg(0, std::ios::end);
    bufsize = inf.tellg();
    //create a buf size
    buf = new char[bufsize];
    inf.seekg(0);
    inf.read(buf, bufsize);
    inf.close();

    int sequence_num(0);
    oh = msgpack::unpack(buf, bufsize, off); //unpack the data to get the object handler
    oh.get().convert(in_line); //get the vector<int> type
    sequence_num = atoi(in_line[0].c_str());
    delete [] buf;

    std::fstream out;
    out.open("output.txt", std::ios::out | std::ios::binary);
    for(int i=0; i< sequence_num; i++) {
	msgpack::sbuffer sbuf;
	std::string line;
	//msg pack array format family
		std::istringstream is(in_line[2*i+1]);
		int dnum;
		is >> line;
		dnum = atoi(line.c_str());
		struct dinfo d_arr[dnum] ;
		for(int j=0; j< dnum; j++) {
			is >> d_arr[j].name;
			is >> d_arr[j].phone;
		}
		is.clear();

		std::sort(d_arr, d_arr+dnum, dsort);

		//read pattern
		is.str(in_line[2*i+2]);
		int pnum;
		is >> line;
		pnum = atoi(line.c_str());
		std::string os;
		while(pnum) {
			is >> line;
			for(int j =0; j< dnum; j++) {
				size_t found;
				found = d_arr[j].phone.find(line);
				if(found != std::string::npos) {
					if(!os.empty())
						os+=" ";
					os+=d_arr[j].name+" "+d_arr[j].phone;
				}
			}
			pnum--;
		}
		out_line.push_back(os);
		is.clear();
	}
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf,out_line);
	out.write(sbuf.data(),sbuf.size());
	out.close();
	return 0;
}
