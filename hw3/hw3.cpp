#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <sstream>
//#include <math.h>
//#include <iostream>
#include <msgpack.hpp>
#include <set>
#include <map>

struct dinfo{
	std::string name;
	std::string phone;
};

struct tinfo{
	struct dinfo *ptr;
//	int id;
	int position;
	tinfo(struct dinfo *pt, int p):ptr(pt), position(p){}
};
bool dsort(const struct dinfo &a, const struct dinfo &b) {
	return (a.phone < b.phone);
}

int main() {

    char *buf = NULL;
    size_t off = 0;
    int bufsize;
    msgpack::object_handle oh;
    std::string line;
    std::vector<std::string> in_line;
    std::vector<std::string> out_line;

    std::fstream inf;
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

    for(int i=0; i< sequence_num; i++) {

	//get patter front set
	std::set<char> pset;
	std::vector<std::string> pattern;
	std::istringstream is(in_line[2*i+2]);
	int pnum;
	is >> line;
	pnum = atoi(line.c_str());
	for(int j=0; j< pnum; j++) {
		is >> line;
		char digit=line.at(0);
		pset.insert(digit);
		pattern.push_back(line);
	}
	is.clear();
	//msg pack array format family
	is.str(in_line[2*i+1]);
	int dnum;
	is >> line;
	dnum = atoi(line.c_str());
	struct dinfo d_arr[dnum];
	std::multimap<char, struct tinfo> table;
	for(int j=0; j< dnum; j++) {
		is >> d_arr[j].name;
		is >> d_arr[j].phone;
		for(int k=0; k < d_arr[j].phone.size(); k++) {
			char digit = d_arr[j].phone.at(k);
			if(pset.count(digit)) {
				struct tinfo item(&d_arr[j],k);
				table.insert(std::make_pair(digit, item));
			}
		}
	}
	is.clear();
	std::sort(d_arr, d_arr+dnum, dsort);

	//read pattern
	std::string os;
	for(std::vector<std::string>::iterator its= pattern.begin(); its != pattern.end(); its++) {
		char digit=(*its).at(0);
		std::multimap<char, struct tinfo>::iterator it,itlow,itup;
		itlow = table.lower_bound(digit);
		itup = table.upper_bound(digit);
		for(it = itlow; it != itup; it++) {
			struct dinfo item = *(it->second.ptr);
			int pos = it->second.position;
			if(pos + (*its).size() <= item.phone.size()-1) {
				std::string s = item.phone.substr(pos, (*its).size());
				if(s.compare(*its) == 0) {
					if(!os.empty())
						os+=" ";
					os+=item.name+" "+item.phone;
				}
			}
		}
	}
	table.clear();
	out_line.push_back(os);
    }
    std::fstream out;
    msgpack::sbuffer sbuf;
    out.open("output.txt", std::ios::out | std::ios::binary);
    msgpack::pack(sbuf,out_line);
    out.write(sbuf.data(),sbuf.size());
    out.close();
    return 0;
}
