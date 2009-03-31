
#include "embeded_web_server.h"

using namespace dlib;
using namespace std;

class web_server : public server::http_1a_c
{

  private:

    ann_service ann_component;

  public:

    web_server(cbir_component& cbir_config) {
      ann_component.init(cbir_config); 
      cout << "test" << endl;
    }

    void on_request (
        const std::string& path,
        std::string& result,
        const map_type& queries,
        const map_type& cookies,
        queue_type& new_cookies,
        const map_type& incoming_headers,
        map_type& response_headers,
        const std::string& foreign_ip,
        const std::string& local_ip,
        unsigned short foreign_port,
        unsigned short local_port
    )
    {
        try
        {
            ostringstream sout;
            // We are going to send back a page that contains an HTML form with two text input fields.
            // One field called name.  The HTML form uses the post method but could also use the get
            // method (just change method='post' to method='get').


            if (path == "/form_handler") {

              int top_k = 10;

              if (queries.is_in_domain("k") && !queries["k"].empty() && atoi(queries["k"].c_str()) > 0) {
                top_k = atoi(queries["k"].c_str());
              }

              int query_mode = 0;
              int index = 0;

              std::string postfix = "";
              std::string feature_string = "";

              if (queries.is_in_domain("postfix") && !queries["postfix"].empty()) {
                postfix = queries["postfix"].c_str();
              }

              if (queries.is_in_domain("flickr_id") && !queries["flickr_id"].empty()) {

                if (queries.is_in_domain("sample")) {
                  feature_string = ann_component.query_test_sample_feature_by_id((queries["flickr_id"] + postfix).c_str());
                  query_mode = 2;
                }
                else {
                  index = ann_component.query_feature_point_by_id((queries["flickr_id"] + postfix).c_str());
                  query_mode = 1;
                }
              }
              else if (queries.is_in_domain("query_id") && !queries["query_id"].empty()) {
                index = atoi(queries["query_id"].c_str());
                query_mode = 1;
              } 

              if (queries.is_in_domain("feature") && !queries["feature"].empty()) {
                feature_string = queries["feature"];
                query_mode = 2;
              }

              std::string ret;

              if (query_mode == 1) {
                ret = ann_component.query_by_index(top_k, index);
              }
              else if (query_mode == 2) {
                ret = ann_component.query_by_feature_vector(top_k, feature_string);
              } 


              //sout << queries["flickr_id"] << " " << queries["query_id"] << " " << queries["k"] << endl;
              //sout << index << " " << top_k << endl;

              sout << ret.c_str() << endl;
            }
            else {
              sout << " <html> <body> "
                << "<form action='/form_handler' method='post'> "
                << "Top K: <input name='k' type='text'><br>  "
                << "Sample point: <input name='query_id' type='text'>"
                << "Flickr photo id: <input name='flickr_id' type='text'> <input name='postfix' type='hidden' value='.jpg'><br>"
                << "Feature vector string: <input name='feature' type='text'> <input type='submit'> "
                << " </form>" << endl; 

              sout << "<br>  path = "         << path << endl;
              sout << "<br>  foreign_ip = "   << foreign_ip << endl;
              sout << "<br>  foreign_port = " << foreign_port << endl;
              sout << "<br>  local_ip = "     << local_ip << endl;
              sout << "<br>  local_port = "   << local_port << endl;
              sout << "</body> </html>";
            }

/*
            if (path == "/form_handler")
            {
                sout << "<h2> Stuff from the query string </h2>" << endl;
                sout << "<br>  user = " << queries["user"] << endl;
                sout << "<br>  pass = " << queries["pass"] << endl;
*/

/*
                // save these form submissions as cookies.  
                string cookie;
                cookie = "user=" + queries["user"]; 
                new_cookies.enqueue(cookie);
                cookie = "pass=" + queries["pass"]; 
                new_cookies.enqueue(cookie);
              
            }
*/

/*
            // Echo any cookies back to the client browser 
            sout << "<h2>Cookies we got back from the server</h2>";
            cookies.reset();
            while (cookies.move_next())
            {
                sout << "<br/>" << cookies.element().key() << " = " << cookies.element().value() << endl;
            }

            sout << "<br/><br/>";
*/

/*
            sout << "<h2>HTTP Headers we sent to the server</h2>";
            // Echo out all the HTTP headers we received from the client web browser
            incoming_headers.reset();
            while (incoming_headers.move_next())
            {
                sout << "<br/>" << incoming_headers.element().key() << ": " << incoming_headers.element().value() << endl;
            }
*/

            //sout << "</body> </html>";

            result = sout.str();
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }
    }

};

web_server* our_web_server;

void thread()
{
    cout << "Press enter to end this program" << endl;
    cin.get();
    // this will cause the server to shut down which will in turn cause 
    // our_web_server.start() to unblock and thus the main() function will terminate.
    our_web_server->clear();
}
 
bool main_loop_for_web_server(http_server& server_config, cbir_component& cbir_config) {

  our_web_server = new web_server(cbir_config);

  try {
    // create a thread that will listen for the user to end this program
    thread_function t(thread);

    our_web_server->set_listening_port(server_config.port);
    our_web_server->start();
  }
  catch (exception& e) {
        cout << e.what() << endl;
        return false;
    }

    return true; 
}


