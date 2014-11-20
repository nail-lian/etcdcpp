/*
 * main.cpp
 * Copyright (C) 2014 wangyongliang <wangyongliang@WANGYL-JD>
 *
 * Distributed under terms of the MIT license.
 */

#include "../include/etcd.h"
#include "../3rdparty/include/rapidjson/stringbuffer.h"
#include "../3rdparty/include/rapidjson/writer.h"
#include <iostream>
#include <map>

using namespace std;
using namespace etcd;
using namespace rapidjson;

int main() {
  vector<Cluster> clusters;
  clusters.push_back(Cluster("127.0.0.1", 4001));
  Client client(clusters);
  Document doc;

  // set values
  bool result = client.SetValue("key", "hello");

  // set value with ttl
  result = client.SetValue("key2", "hello", 3);
  string value;
  // get values;
  result = client.GetValue(value, "key");
  cout << "Get value for key: " << value << endl;
  // delete value
  result = client.DeleteValue("key");

  result = client.MakeDir("dir");
  result = client.SetValue("dir/a", "a");
  result = client.MakeDir("dir/sub");
  result = client.SetValue("dir/sub/b", "b");

  map<string, string> values;
  // list values in dir include subdir
  result = client.ListDir(values, "dir", true);
  for (map<string, string>::iterator it = values.begin(); it != values.end(); it ++) {
    cout << it->first << "->" << it->second << endl;
  }

  // remove dir include all files
  result = client.RemoveDir("dir", true);
  return 0;

}


