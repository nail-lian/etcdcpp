/*
 * etcd.h
 * Copyright (C) 2014 wangyongliang <wangyongliang@WANGYL-JD>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ETCD_H
#define ETCD_H

#include <string>
#include <vector>
#include "../3rdparty/include/stout/try.hpp"
#include "../3rdparty/include/rapidjson/document.h"

namespace etcd {
class Cluster {
public:
  Cluster(const std::string& host, int32_t port);
  virtual ~Cluster();
  std::string GetHost() const;
  int32_t GetPort() const;
private:
  std::string host;
  int32_t port;
};


class GetOption{
public:
  GetOption();
  ~GetOption();
  static GetOption Default();
  bool IsDir() const {return is_dir;}
  bool IsWait() const {return is_wait;}
  bool IsRecursive() const {return is_recursive;}
private:
  bool is_dir;
  bool is_wait;
  bool is_recursive;
};

class SetOption {
public:
  SetOption();
  ~SetOption();
  static SetOption Default();
  bool IsSetTTL() const {return is_set_ttl;}
  bool IsUnSetTTL() const {return is_unset_ttl;}
  void SetTTL(unsigned int ttl_) {
    is_set_ttl = true;
    ttl = ttl_;
    is_unset_ttl = false;
  }
  void UnsetTTL() {
    is_unset_ttl = true;
    is_set_ttl = false;
  }
  bool IsDir() const {
    return is_dir;
  }
  unsigned int GetTTL() const {return ttl;}
private:
  unsigned int ttl;
  bool is_set_ttl;
  bool is_unset_ttl;
  bool is_dir;
};

class DeleteOption {
public:
  DeleteOption();
  ~DeleteOption();
  static DeleteOption Default();
  bool IsDir() const {return is_dir;}
private:
  bool is_dir;
};

class Node {
public:
  unsigned int createdIndex;
  unsigned int modifiedIndex;
  std::string key;
  std::string value;
};


class Client {
public:
  explicit Client(const std::vector<Cluster>& clusters);
  //explicit Client(const std::string& file);
  virtual ~Client();
  // Get the key
  Try<bool> Get(
      rapidjson::Document& _return,
      const std::string& key,
      const GetOption& option = GetOption::Default());

  Try<bool> Set(
      rapidjson::Document& _return,
      const std::string& key,
      const std::string& value,
      const SetOption& = SetOption::Default());

  Try<bool> Delete(
      rapidjson::Document& _return,
      const std::string& key,
      const DeleteOption& = DeleteOption::Default());

  //Try<rapidjson::Document> CompareAndSwap();
  //Try<rapidjson::Document> CompareAndDelete();
  //Try<rapidjson::Document> MakeDir(
      //const std::string& key,
      //const SetOption& option=SetOption::Default());

  //Try<rapidjson::Document> ListDir(const std::string& key, const GetOption& option=GetOption::Default());
  //Try<rapidjson::Document> RemoveDir(const std::string& key, const DeleteOption& option=DeleteOption::Default());
private:
  std::vector<Cluster> clusters;
};

}


#endif /* !ETCD_H */
