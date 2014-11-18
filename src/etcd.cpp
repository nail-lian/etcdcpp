/*
 * etcd.cpp
 * Copyright (C) 2014 wangyongliang <wangyongliang@WANGYL-JD>
 *
 * Distributed under terms of the MIT license.
 */

#include "../include/etcd.h"
#include "../3rdparty/include/stout/error.hpp"
#include <curl/curl.h>
#include <sstream>
#include <map>

namespace etcd {
using std::string;
using std::vector;
using std::map;
using std::stringstream;
using rapidjson::Document;

namespace internal {
class Curl {
public:
  Curl() {
    curl = curl_easy_init();
  }

  virtual ~Curl() {
    if (curl) {
      curl_easy_cleanup(curl);
    }
  }
  CURL* curl;
};

class Builder {
public:
  string Build(const ::etcd::Cluster& cluster, const string& key, const GetOption& option) {
    stringstream _return;
    _return << cluster.GetHost() << ":" << cluster.GetPort() << "/v2/keys/" << key;
    if (option.is_dir)
      _return << "/";

    map<string, string> param;
    param.clear();
    if (option.wait)
      param["wait"] = "true";
    if (option.recursive)
      param["recursive"] = "true";
    AppendParam(_return, param);
    return _return.str();
  }

  void AppendParam(stringstream& _return, const map<string, string>& param) {
    if (param.size() == 0)
      return;
    map<string, string>::const_iterator it;
    bool first = true;
    for (it = param.begin(); it != param.end(); it ++) {
      if (first) {
        _return << "?";
        first = false;
      }
      _return << it->first << "=" << it->second;
    }
  }

};

int WriteToString(char* data, size_t size, size_t len, string* _return) {
  if (_return != NULL) {
    _return->append(data, size * len);
    return size * len;
  } else {
    return 0;
  }
}



} // namespace internal

/*
 * GetOption::
 */
GetOption::GetOption():is_dir(false), wait(false), recursive(false) {}
GetOption::~GetOption(){}
GetOption GetOption::Default() {return GetOption();}

/*
 * SetOption
 */
SetOption::SetOption():ttl(-1), is_dir(false){}
SetOption::~SetOption() {}
SetOption SetOption::Default() {return SetOption();}

/*
 * DeleteOption
 */
DeleteOption::DeleteOption():is_dir(false) {}
DeleteOption::~DeleteOption() {}
DeleteOption DeleteOption::Default() {return DeleteOption();}


/*
 * Cluster
 */
Cluster::Cluster(const string& host_, int32_t port_): host(host_), port(port_) {}
Cluster::~Cluster() {}
string Cluster::GetHost() const {return host;}
int32_t Cluster::GetPort() const {return port;}

/*
 * Client
 */

Client::Client(const vector<Cluster>& clusters_):clusters(clusters_) {}

Client::~Client() {}


Try<Document> Client::Get(const string& key, const GetOption& option) {
  if (clusters.size()) {
    char buffer[1024];
    stringstream sin;
    internal::Builder builder;
    CURLcode code;
    for (size_t i = 0; i < clusters.size(); i ++) {
      sin.str("");
      string url = builder.Build(clusters[i], key, option);
      internal::Curl curl;
      string buffer;

      curl_easy_setopt(curl.curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl.curl, CURLOPT_WRITEFUNCTION, &internal::WriteToString);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEDATA, &buffer);
      code = curl_easy_perform(curl.curl);
      if (code == CURLE_OK) {
        Document doc;
        doc.Parse(buffer.c_str());
        return doc;
      }
    }
    return ErrnoError();
  } else {
    return ErrnoError("clusters is empty");
  }
}
} // namespace etcd

