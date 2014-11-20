/*
 * client.cpp
 * Copyright (C) 2014 wangyongliang <wangyongliang.wyl@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */


#include "../include/etcd.h"

#include "../3rdparty/include/gtest/gtest.h"
#include "../3rdparty/include/rapidjson/stringbuffer.h"
#include "../3rdparty/include/rapidjson/writer.h"

namespace {
using namespace etcd;
using namespace rapidjson;
using namespace std;

void PrintDocument(Document &doc) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    cout << buffer.GetString() << endl;
}

class ClientTest: public testing::Test {
protected:
  vector<Cluster> clusters;

  ClientTest() {
    clusters.push_back(Cluster("127.0.0.1", 4001));
  }
};

TEST_F(ClientTest, Basic) {
  Client client(clusters);
  Document doc;
  EXPECT_TRUE(client.Set(doc, "key", "hello"));

  // Get
  EXPECT_TRUE(client.Get(doc, "key"));

  // Delete
  EXPECT_TRUE(client.Delete(doc, "key"));

  // Get again
  EXPECT_TRUE(client.Get(doc, "key"));
}

TEST_F(ClientTest, SetFailed) {
  vector<Cluster> clusters;
  clusters.push_back(Cluster("127.0.0.1", 4002));
  Client client(clusters);
  Document doc;
  EXPECT_FALSE(client.Set(doc, "key", "hello"));
}

TEST_F(ClientTest, SetTTL) {
  Client client(clusters);
  SetOption option;
  option.SetTTL(5);
}

TEST_F(ClientTest, Value) {
  Client client(clusters);
  EXPECT_TRUE(client.SetValue("key", "value"));
  string value;
  EXPECT_TRUE(client.GetValue(value, "key"));
  EXPECT_TRUE(value == "value");

  // Get un-exist value
  EXPECT_FALSE(client.GetValue(value, "key2"));

  EXPECT_TRUE(client.DeleteValue("key"));
  // Delete un-existed key
  EXPECT_FALSE(client.DeleteValue("key3"));
  EXPECT_FALSE(client.GetValue(value, "key"));

  // Test ttl
  EXPECT_TRUE(client.SetValue("key", "value", 3));
  sleep(4);
  EXPECT_FALSE(client.GetValue(value, "key"));

  // test unset ttl
  EXPECT_TRUE(client.SetValue("key", "value", 3));
  EXPECT_TRUE(client.UnsetTTL("key", "value"));
  sleep(5);
  EXPECT_TRUE(client.GetValue(value, "key"));
  EXPECT_TRUE(value == "value");
}

TEST_F(ClientTest, SetValue) {
  Client client(clusters);
  EXPECT_TRUE(client.SetValue("/a/b/c", "d"));
  string value;
  EXPECT_TRUE(client.GetValue(value, "/a/b/c"));
  EXPECT_TRUE(value == "d");

  EXPECT_FALSE(client.DeleteValue("/a"));
  EXPECT_TRUE(client.GetValue(value, "/a/b/c"));
}

TEST_F(ClientTest, MakeDir) {
  Client client(clusters);
  EXPECT_TRUE(client.MakeDir("dir"));

  EXPECT_TRUE(client.SetValue("dir/a", "a"));
  string value;
  EXPECT_TRUE(client.GetValue(value, "dir/a"));
  EXPECT_TRUE(value == "a");
  EXPECT_TRUE(client.SetValue("dir/b", "b"));

  map<string, string> values;
  EXPECT_TRUE(client.ListDir(values, "dir/"));
  EXPECT_TRUE(values.size() == 2);
  EXPECT_TRUE(values["/dir/a"] == "a");
  EXPECT_TRUE(values["/dir/b"] == "b");

  EXPECT_FALSE(client.RemoveDir("dir"));
  EXPECT_TRUE(client.RemoveDir("dir", true));
}

TEST_F(ClientTest, ListDir) {
  Client client(clusters);
  EXPECT_TRUE(client.MakeDir("dir"));

  EXPECT_TRUE(client.SetValue("/dir/a", "a"));
  EXPECT_TRUE(client.SetValue("/dir/b", "b"));

  EXPECT_TRUE(client.MakeDir("dir/subdir"));
  EXPECT_TRUE(client.SetValue("/dir/subdir/a", "a"));

  map<string, string> values;
  EXPECT_TRUE(client.ListDir(values, "dir", true));
  EXPECT_TRUE(values.size() == 3);
  EXPECT_TRUE(values["/dir/a"] == "a");
  EXPECT_TRUE(values["/dir/b"] == "b");
  EXPECT_TRUE(values["/dir/subdir/a"] == "a");

  EXPECT_TRUE(client.RemoveDir("/dir", true));

}


}


