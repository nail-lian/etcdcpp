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
#include <algorithm>
#include <iostream>

namespace etcd {
using std::string;
using std::vector;
using std::map;
using std::stringstream;
using rapidjson::Document;
using std::pair;
using std::cout;
using std::endl;

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
    if (option.IsDir())
      _return << "/";

    map<string, string> param;
    param.clear();
    if (option.IsWait())
      param["wait"] = "true";
    if (option.IsRecursive())
      param["recursive"] = "true";
    AppendParam(_return, param);
    return _return.str();
  }

  pair<string, string> Build(const Cluster& cluster, const string& key, const string& value, const SetOption& option) {
    stringstream _return;
    _return << cluster.GetHost() << ":" << cluster.GetPort() << "/v2/keys/" << key;
    if (option.IsDir())
      _return << "/";
    pair<string, string> result;
    result.first = _return.str();
    _return.str("");
    map<string, string> param;
    param["value"] = value;

    if (option.IsSetTTL()) {
      _return.str("");
      _return << option.GetTTL();
      param["ttl"] = _return.str();
    }

    if (option.IsUnSetTTL()) {
      param["ttl"] = "";
    }
    _return.str("");
    AppendPostField(_return, param);
    result.second = _return.str();
    return result;
  }

  string Build(const Cluster& cluster, const string& key, const DeleteOption& option) {
    stringstream _return;
    _return << cluster.GetHost() << ":" << cluster.GetPort() << "/v2/keys/" << key;
    if (option.IsDir()) {
      _return << "?dir=true";
    }
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

  void AppendPostField(stringstream& _return, const map<string, string>& param) {
    if (param.size() == 0)
      return;
    map<string, string>::const_iterator it;
    bool first = true;
    for (it = param.begin(); it != param.end(); it ++) {
      if (first) {
        first = false;
      } else {
        _return << ";";
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
GetOption::GetOption():is_dir(false), is_wait(false), is_recursive(false) {}
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


Try<bool> Client::Get(Document& _return, const string& key, const GetOption& option) {
  if (clusters.size()) {
    internal::Builder builder;
    CURLcode code;
    for (size_t i = 0; i < clusters.size(); i ++) {
      string url = builder.Build(clusters[i], key, option);
      internal::Curl curl;
      string buffer;

      curl_easy_setopt(curl.curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl.curl, CURLOPT_WRITEFUNCTION, &internal::WriteToString);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEDATA, &buffer);
      code = curl_easy_perform(curl.curl);
      if (code == CURLE_OK) {
        _return.Parse(buffer.c_str());
        return true;
      }
    }
    return false;
  } else {
    return ErrnoError("clusters is empty");
  }
}

Try<bool> Client::Set(Document& _return, const string& key, const string& value, const SetOption& option) {
  if (clusters.size()) {
    internal::Builder builder;
    CURLcode code;
    for (size_t i = 0; i < clusters.size(); i ++) {
      pair<string, string> result = builder.Build(clusters[i], key, value, option);
      internal::Curl curl;
      string buffer;

      curl_easy_setopt(curl.curl, CURLOPT_CUSTOMREQUEST, "PUT");
      curl_easy_setopt(curl.curl, CURLOPT_URL, result.first.c_str());
      curl_easy_setopt(curl.curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl.curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEFUNCTION, &internal::WriteToString);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEDATA, &buffer);

      // send post data
      curl_easy_setopt(curl.curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl.curl, CURLOPT_POSTFIELDS, result.second.c_str());

      code = curl_easy_perform(curl.curl);
      if (code == CURLE_OK) {
        _return.Parse(buffer.c_str());
        return true;
      } else {
        cout << curl_easy_strerror(code) << endl;
      }
    }
    return false;
  } else {
    return Error("clusters is empty");
  }
}

Try<bool> Client::Delete(Document& _return, const string& key, const DeleteOption& option) {
  if (clusters.size()) {
    internal::Builder builder;
    CURLcode code;
    for (size_t i = 0; i < clusters.size(); i ++) {
      string url = builder.Build(clusters[i], key, option);
      internal::Curl curl;
      string buffer;

      curl_easy_setopt(curl.curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      curl_easy_setopt(curl.curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl.curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl.curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEFUNCTION, &internal::WriteToString);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEDATA, &buffer);

      code = curl_easy_perform(curl.curl);
      if (code == CURLE_OK) {
        _return.Parse(buffer.c_str());
        return true;
      }
    }
    return false;
  } else {
    return Error("clusters is empty");
  }
}

} // namespace etcd

