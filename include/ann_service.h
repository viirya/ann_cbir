
#ifndef ANN_SERVER

#define ANN_SERVER

#include "featureloader.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ANN/ANN.h>
#include "cbir_config.h"

class ann_service {

  private:

    feature_loader loader;  

    int k;
    int nPts;
    double eps;
    int dim;
    int max_nPts;
    int max_query_num;

    ANNpointArray dataPts; 
    ANNpoint queryPt;     
    ANNidxArray nnIdx;   
    ANNdistArray dists;       
    ANNkd_tree* kdTree;

    int readPt(feature_loader &loader, ANNpointArray &pts) {

      for(int i = 0; i < loader.feature_list.size(); i++) {
        feature_item* featureitem = &(loader.feature_list.at(i));
      
        //ANNpoint* p = &pts[i];

        for (int j = 0; j < dim; j++) {
           pts[i][j] = featureitem->feature[j];
        }
     }

      return loader.feature_list.size();
    }

    void printPt(ostream &out, ANNpoint p) {
      out << "(" << p[0];
      for (int i = 1; i < dim; i++) {
        out << ", " << p[i];
      }
      out << ")\n";
    }

  public:

    ann_service() {
    
      k = 10;
      nPts = 0;
      eps = 0;
      dim = 0;
      max_nPts = 15000;
      max_query_num = 10000;
      queryPt = NULL;

    }

    int init(cbir_component& cbir_config) {

      dim = cbir_config.dim;
      nPts = 0;

      
      if (load_sample_files(cbir_config.sample_files) && load_filter_files(cbir_config.filter_files) && load_feature_files(cbir_config.feature_files)) {
        cout << "allocating memory space ....." << endl;
        dataPts = annAllocPts(max_nPts, dim);
        cout << "reading data ....." << endl;
        nPts = readPt(loader, dataPts);
        cout << "Data Points: " << nPts << endl;

        /*
        for (int i = 0; i < nPts; i++) {
          printPt(cout, dataPts[i]);
        }
        */
        
        kdTree = new ANNkd_tree(dataPts, nPts, dim);
      }
    }

    std::string query_test_sample_feature_by_id(const char* id) {

      std::ostringstream osstream;

      osstream << "";

      for(int i = 0; i < loader.sample_feature_list.size(); i++) {
        feature_item featureitem = loader.sample_feature_list.at(i);
        if (featureitem.id.compare(id) == 0) {
          osstream << featureitem.feature[0];
          for (int j = 1; j < dim; j++) {
            osstream << " " << featureitem.feature[j];
          }
          break;
        }
      }

      return osstream.str();

    }


    std::string query_feature_by_id(const char* id) {

      std::ostringstream osstream;

      osstream << "";

      for(int i = 0; i < loader.feature_list.size(); i++) {
        feature_item featureitem = loader.feature_list.at(i);
        if (featureitem.id.compare(id) == 0) {
          osstream << featureitem.feature[0];
          for (int j = 1; j < dim; j++) {
            osstream << " " << featureitem.feature[j];
          }
          break;
        }
      }

      return osstream.str();

    }

    int query_feature_point_by_id(const char* id) {

      for(int i = 0; i < loader.feature_list.size(); i++) {
        feature_item featureitem = loader.feature_list.at(i);
        if (featureitem.id.compare(id) == 0) {
          return i;
        }
      }
      return -1;

    }
 
   bool load_sample_files(std::vector <std::string> files) {
      cout << "loading sample files....." << endl;
      for (int i = 0; i < files.size(); i++) {
        cout << "  loading " << files[i] << endl;
        loader.loadsamplefiles(files[i].c_str());
      }
      cout << "finish." << endl;
      return true;
    }

 
   bool load_filter_files(std::vector <std::string> files) {
      cout << "loading filter files....." << endl;
      for (int i = 0; i < files.size(); i++) {
        cout << "  loading " << files[i] << endl;
        loader.loadfilterfiles(files[i].c_str());
      }
      cout << "finish." << endl;
      return true;
    }

   
    bool load_feature_files(std::vector <std::string> files) {
      cout << "loading feature files....." << endl;
      for (int i = 0; i < files.size(); i++) {
        cout << "  loading " << files[i] << endl;
        loader.loadfromfile(files[i].c_str(), dim);
      }
      cout << "finish." << endl;
      return true;
    }

    std::string query_by_feature_vector(int num, std::string feature, double custom_eps = 0.0) {

      std::istringstream feature_str(feature);

      std::string output;
      std::ostringstream osstream;
      
      if (feature == "") {
        osstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        osstream << "<results>\n" << "</results>\n";
        return osstream.str();
      }

      if (queryPt != NULL) {
        annDeallocPt(queryPt);
      }

      ANNcoord init_value = 0;
      queryPt = annAllocPt(dim, init_value);

      for (int i = 0; i < dim; i++) {
        feature_str >> queryPt[i];
        if (feature_str.eof() || feature_str.fail()) {
          break;
        }
      }

      output = process_query(num, custom_eps);

      annDeallocPt(queryPt);

      return output;

    }

    std::string query_by_index(int num, int query_point_inside, double custom_eps = 0.0) {

      std::string output;
      std::ostringstream osstream;

      if (query_point_inside < 0 || num < 0) {
        osstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        osstream << "<results>\n" << "</results>\n";
        return osstream.str();
      }

      query_point_inside %= nPts;

      if (queryPt != NULL) {
        annDeallocPt(queryPt);
      }

      queryPt = annAllocPt(dim);

      for (int i = 0; i < dim; i++) {      
        queryPt[i] = dataPts[query_point_inside][i];
      }

      output = process_query(num, query_point_inside, custom_eps);

      annDeallocPt(queryPt);

      return output;

    }


    std::string process_query(int num, int query_point_inside = -1, double custom_eps = 0.0) {

      std::string output;
      std::ostringstream osstream;
/*
      if (query_point_inside < 0 || num < 0) {
        osstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        osstream << "<results>\n" << "</results>\n";
        return osstream.str();
      }
*/  
      k = num % max_query_num;
/*
      query_point_inside %= nPts;

      queryPt = annAllocPt(dim);
*/      
      nnIdx = new ANNidx[k];
      dists = new ANNdist[k];

//      queryPt = dataPts[query_point_inside];

      if (queryPt != NULL) {
        kdTree->annkSearch(queryPt, k, nnIdx, dists, custom_eps);
      }

      cout << "\tNN:\tIndex\tDistance\n";
      osstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

      osstream << "<results>\n";

      if (query_point_inside > 0) {
        feature_item featureitem = loader.feature_list.at(query_point_inside);
        osstream << "<query>\n" << "<id>" << featureitem.id << "</id>\n" << "<index>" << query_point_inside << "</index>\n" << "</query>\n";
      }

      //osstream << "NN:\tIndex\tDistance<br/>";
      
      for (int i = 0; i < k; i++) {                   // print summary
        dists[i] = sqrt(dists[i]);                      // unsquare distance
        cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << "\n";

        feature_item featureitem = loader.feature_list.at(nnIdx[i]);
        osstream << "<item>\n<rank>" << i << "</rank>" << "\n" << "<id>" << featureitem.id << "</id>\n" << "<index>" << nnIdx[i] << "</index>" << "\n" << "<distance>" << dists[i] << "</distance>\n" << "</item>\n";
      }

      osstream << "</results>\n";
  
      delete [] nnIdx;
      delete [] dists;
      
      return osstream.str();

    }
    
    ~ann_service() {
      delete kdTree;
      annClose();
    }

};

#endif

