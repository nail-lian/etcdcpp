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
  bool is_dir;
  bool wait;
  bool recursive;
};

class SetOption {
public:
  SetOption();
  ~SetOption();
  static SetOption Default();
  int32_t ttl;
  bool is_dir;
};

class DeleteOption {
public:
  DeleteOption();
  ~DeleteOption();
  static DeleteOption Default();
  bool is_dir;
};


class Client {
public:
  explicit Client(const std::vector<Cluster>& clusters);
  //explicit Client(const std::string& file);
  virtual ~Client();
  // Get the key
  Try<rapidjson::Document> Get(
      const std::string& key,
      const GetOption& option = GetOption::Default());

  Try<rapidjson::Document> Set(
      const std::string& key,
      const std::string& value,
      const SetOption& = SetOption::Default());

  Try<rapidjson::Document> Delete(
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
