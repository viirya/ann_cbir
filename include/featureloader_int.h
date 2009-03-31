
#include <iostream>
#include <vector>
#include <map>

#pragma once

using namespace std;

class feature_item {

public:
        std::string id;
	int* feature;

	feature_item(int length) {
          feature = new int[length];
	}

        void load_id(char* data) {
          id = data;
        }

	void load_feature(int* in, int length) {
  	  for(int i = 0; i < length; i++){
	    feature[i] = in[i];
	  }
	}
};

class feature_loader {

private:
//        map<const char*, int> filters;
        map<std::string, int> filters;

public:
	vector<feature_item> feature_list;

	bool loadfromfile(const char* fileName, int length);
        bool loadfilterfiles(const char* filename);

	feature_loader();
	~feature_loader();

};

