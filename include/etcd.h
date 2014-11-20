/*
 * etcd.h
 * Copyright (C) 2014 wangyongliang <wangyongliang.wyl@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ETCD_H
#define ETCD_H

#include <string>
#include <vector>
#include <map>
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
  virtual ~GetOption();
  static GetOption Default();
  bool IsDir() const {return is_dir;}
  bool IsWait() const {return is_wait;}
  bool IsRecursive() const {return is_recursive;}
  bool IsConsistent() const {return is_consistent;}
  bool IsQuorum() const {return is_quorum;}
  void SetConsistent(bool flag) {is_consistent=flag;}
  void SetQuorum(bool flag) {is_quorum = flag;}
  void SetWait(bool flag) {is_wait = flag;}
  void SetDir(bool flag) {is_dir = flag;}
  void SetRecursive(bool flag) {is_recursive = flag;}
private:
  bool is_dir;
  bool is_wait;
  bool is_recursive;
  bool is_consistent;
  bool is_quorum;
};

class SetOption {
public:
  SetOption();
  virtual ~SetOption();
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
  void SetPrevInde(unsigned int index) {
    prevIndex = index;
  }

  void SetPrevValue(const std::string& value) {
    prevValue = value;
  }

  void SetPrevExist(bool exist) {
    prevExist = exist;
  }
  unsigned int GetTTL() const {return ttl;}
  void SetDir(bool flag) { is_dir = flag;}
private:
  unsigned int ttl;
  bool is_set_ttl;
  bool is_unset_ttl;
  bool is_dir;
  unsigned int prevIndex;
  std::string prevValue;
  bool prevExist;
};

class DeleteOption {
public:
  DeleteOption();
  virtual ~DeleteOption();
  static DeleteOption Default();
  bool IsDir() const {return is_dir;}
  bool IsRecursive() const {return is_recursive;}
  void SetDir(bool flag) {is_dir = flag;}
  void SetPrevIndex(unsigned int index) {prevIndex = index;}
  void SetPrevValue(const std::string& value) {prevValue = value;}
  void SetPrevExist(bool flag) {prevExist = flag;}
  void SetRecursive(bool flag) {is_recursive = flag;}
private:
  bool is_dir;
  bool is_recursive;
  unsigned int prevIndex;
  std::string prevValue;
  bool prevExist;
};


class Client {
public:
  explicit Client(const std::vector<Cluster>& clusters);
  //explicit Client(const std::string& file);
  virtual ~Client();
  // Get the key
  bool Get(rapidjson::Document& _return,
      const std::string& key,
      const GetOption& option = GetOption::Default());

  bool Set(rapidjson::Document& _return,
      const std::string& key,
      const std::string& value,
      const SetOption& = SetOption::Default());

  bool Delete(rapidjson::Document& _return,
      const std::string& key,
      const DeleteOption& = DeleteOption::Default());

  // shortcuts
  bool GetValue(std::string& _return, const std::string& key, bool consistent=false, bool quorum=false, bool wait=false);

  bool WatchValue(std::string& _return, const std::string& key, bool consistent=false, bool quorum=false);

  bool SetValue(const std::string& key, const std::string& value);
  bool SetValue(const std::string& key, const std::string& value, unsigned int ttl);
  bool UnsetTTL(const std::string& key, const std::string& value);
  bool DeleteValue(const std::string& key);
  //bool CompareAndSwap(const std::string& key, const std::string& value, unsigned int prevIndex);
  //bool CompareAndDelete(const std::string& key, unsigned int prevIndex);

  // directory
  bool MakeDir(const std::string& key);
  bool ListDir(std::map<std::string, std::string>& _return, const std::string& key, bool recursive=false);
  bool RemoveDir(const std::string& key, bool recursive=false);
private:
  std::vector<Cluster> clusters;
};

}


#endif /* !ETCD_H */
