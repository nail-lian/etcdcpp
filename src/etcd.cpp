/*
 * etcd.cpp
 * Copyright (C) 2014 wangyongliang <wangyongliang.wyl@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "../include/etcd.h"
#include <curl/curl.h>
#include <sstream>
#include <map>
#include <algorithm>
#include <iostream>
#include <exception>
#include "../3rdparty/include/rapidjson/stringbuffer.h"
#include "../3rdparty/include/rapidjson/writer.h"

namespace etcd {
using std::string;
using std::vector;
using std::map;
using std::stringstream;
using rapidjson::Document;
using rapidjson::Value;
using rapidjson::StringBuffer;
using rapidjson::Writer;
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

class strings {
public:
  static string RemovePrefix(const string& from, const string& sub="/") {
    if (from.find(sub) == 0) {
      return from.substr(sub.size());
    }
    return from;
  }

  static string RemoveSuffix(const string& from, const string& sub = "/") {
    if (from.rfind(sub) == from.size() - sub.size()) {
      return from.substr(0, from.size() - sub.size());
    }
    return from;
  }

  static string Trim( const string& from, const string& chars = " \t\n\r") {
    size_t start = from.find_first_not_of(chars);
    size_t end = from.find_last_not_of(chars);
    if (start == std::string::npos) { // Contains only characters in chars.
      return "";
    }
    return from.substr(start, end + 1 - start);
  }

};

class Url {
public:
  static string Join(const string& path1, const string& path2) {
    return strings::RemoveSuffix(path1) + "/" + strings::RemovePrefix(path2);
  }

  static string Join(const string& path1, const string& path2, const string& path3) {
    return Join(path1, Join(path2, path3));
  }
};

class Builder {
public:
  string Build(const ::etcd::Cluster& cluster, const string& key, const GetOption& option) {
    stringstream _return;
    _return << cluster.GetHost() << ":" << cluster.GetPort();
    _return << Url::Join("/v2/keys/", key, option.IsDir()? "/" : "");

    map<string, string> param;
    param.clear();
    if (option.IsWait())
      param["wait"] = "true";
    if (option.IsRecursive())
      param["recursive"] = "true";
    if (option.IsQuorum())
      param["quorum"] = "true";
    if (option.IsConsistent())
      param["consistent"] = "true";
    AppendParam(_return, param);
    return _return.str();
  }

  pair<string, string> Build(const Cluster& cluster, const string& key, const string& value, const SetOption& option) {
    stringstream _return;
    _return << cluster.GetHost() << ":" << cluster.GetPort() ;
    _return << Url::Join("/v2/keys/",  key, option.IsDir() ?  "/" : "");
    pair<string, string> result;
    result.first = _return.str();
    _return.str("");
    map<string, string> param;

    if (option.IsDir())
      param["dir"] = "true";
    else
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
    _return << cluster.GetHost() << ":" << cluster.GetPort();
    _return << Url::Join("/v2/keys", key);
    map<string, string> param;
    if (option.IsDir()) {
      param["dir"] = "true";
    }
    if (option.IsRecursive()) {
      param["recursive"] = "true";
    }
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
      } else {
        _return << "&";
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

void ListDirectoryKV(map<string, string>& _return, const ::rapidjson::Value& node) {
  if (node.HasMember("dir")) {
    if (node.HasMember("nodes")) {
      const ::rapidjson::Value& nodes = node["nodes"];
      for (size_t i = 0; i < nodes.Size(); i ++) {
        ListDirectoryKV(_return, nodes[i]);
      }
    }
  } else {
    _return[node["key"].GetString()] = node["value"].GetString();
  }
}
void DocumentToDebugString(Document& doc) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    cout << buffer.GetString() << endl;
}

} // namespace internal

/*
 * GetOption::
 */
GetOption::GetOption():
  is_dir(false), is_wait(false), is_recursive(false),

  is_consistent(false), is_quorum(false) {}
GetOption::~GetOption(){}
GetOption GetOption::Default() {return GetOption();}

/*
 * SetOption
 */
SetOption::SetOption():
  ttl(-1),
  is_set_ttl(false), is_unset_ttl(false),
  is_dir(false),
  prevIndex(-1), prevValue(""), prevExist(false){}
SetOption::~SetOption() {}
SetOption SetOption::Default() {return SetOption();}

/*
 * DeleteOption
 */
DeleteOption::DeleteOption():
  is_dir(false), is_recursive(false),
  prevIndex(-1), prevValue(""), prevExist(false){}
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


bool Client::Get(Document& _return, const string& key, const GetOption& option) {
  if (clusters.size()) {
    internal::Builder builder;
    CURLcode code;
    long http_code = 0;
    for (size_t i = 0; i < clusters.size(); i ++) {
      string url = builder.Build(clusters[i], key, option);
      internal::Curl curl;
      string buffer;

      curl_easy_setopt(curl.curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl.curl, CURLOPT_WRITEFUNCTION, &internal::WriteToString);
      curl_easy_setopt(curl.curl, CURLOPT_WRITEDATA, &buffer);
      code = curl_easy_perform(curl.curl);
      if (code == CURLE_OK) {
        http_code = 0;
        curl_easy_getinfo(curl.curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code) {
          _return.Parse(buffer.c_str());
          return true;
        }
      }
    }
  }
  return false;
}

bool Client::Set(Document& _return, const string& key, const string& value, const SetOption& option) {
  if (clusters.size()) {
    internal::Builder builder;
    CURLcode code;
    long http_code = 0;
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
        http_code = 0;
        curl_easy_getinfo(curl.curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code / 10 == 20) {
          _return.Parse(buffer.c_str());
          return true;
        }
      }
    }
  }
  return false;
}

bool Client::Delete(Document& _return, const string& key, const DeleteOption& option) {
  if (clusters.size()) {
    internal::Builder builder;
    CURLcode code;
    long http_code = 0;
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
        http_code = 0;
        curl_easy_getinfo(curl.curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200) {
          _return.Parse(buffer.c_str());
          return true;
        }
      }
    }
  }
  return false;
}

bool Client::GetValue(string& _return, const string& key, bool consistent, bool quorum, bool wait) {
  Document doc;
  GetOption option;
  option.SetConsistent(consistent);
  option.SetQuorum(quorum);
  option.SetWait(wait);
  if (Get(doc, key, option)) {
    //internal::DocumentToDebugString(doc);
    if (doc.IsObject() && doc.HasMember("node") && doc["node"].HasMember("value")) {
      _return = doc["node"]["value"].GetString();
      return true;
    }
    return false;
  } else {
    return false;
  }
}

bool Client::WatchValue(string& _return, const string& key, bool consistent, bool quorum) {
  return GetValue(_return,key, consistent, quorum, true);
}

bool Client::SetValue(const string& key, const string& value) {
  SetOption option;
  Document doc;
  if (Set(doc, key, value, option)) {
    //internal::DocumentToDebugString(doc);
    return doc.IsObject() && !doc.HasMember("errorcode");
  }
  return false;
}

bool Client::SetValue(const string& key, const string& value, unsigned int ttl) {
  SetOption option;
  Document doc;
  option.SetTTL(ttl);
  //option.SetPrevExist(true);
  if (Set(doc, key, value, option)) {
    //internal::DocumentToDebugString(doc);
    return doc.IsObject() && !doc.HasMember("errorcode");
  }
  return false;
}

bool Client::UnsetTTL(const string& key, const string& value) {
  SetOption option;
  option.UnsetTTL();
  option.SetPrevExist(true);
  Document doc;
  if (Set(doc, key, value, option)) {
    return doc.IsObject() && !doc.HasMember("errorcode");
  }
  return false;
}

bool Client::DeleteValue(const string& key) {
  DeleteOption option;
  Document doc;
  if (Delete(doc, key, option)) {
    //internal::DocumentToDebugString(doc);
    return doc.IsObject() && !doc.HasMember("errorcode") && !doc.HasMember("errorCode");
  }
  return false;
}


bool Client::MakeDir(const std::string& key) {
  SetOption option;
  option.SetDir(true);
  Document doc;
  if (Set(doc, key, "", option)) {
    return doc.IsObject() && !doc.HasMember("errorcode");
  }
  return false;

}

bool Client::ListDir(map<string, string>& _return, const string& key, bool recursive) {
  GetOption option;
  option.SetDir(true);
  option.SetRecursive(recursive);
  Document doc;
  if (Get(doc, key, option)) {
    if (doc.IsObject() && doc.HasMember("node")) {
      const Value& node = doc["node"];
      internal::ListDirectoryKV(_return, node);
      return true;
    }
  }
  return false;
}

bool Client::RemoveDir(const string& key, bool recursive) {
  DeleteOption option;
  option.SetDir(true);
  option.SetRecursive(recursive);
  Document doc;
  if (Delete(doc, key, option)) {
    if (doc.IsObject() && doc.HasMember("node")) {
      return true;
    }
  }
  return false;
}

} // namespace etcd

