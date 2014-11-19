/*
 * client.cpp
 * Copyright (C) 2014 wangyongliang <wangyongliang@WANGYL-JD>
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
  Try<bool> result = client.Set(doc, "key", "hello");
  EXPECT_TRUE(result.isSome());
  EXPECT_TRUE(result.get());

  // Get
  result = client.Get(doc, "key");
  EXPECT_TRUE(result.isSome());
  EXPECT_TRUE(result.get());

  // Delete
  result = client.Delete(doc, "key");
  EXPECT_TRUE(result.isSome());
  EXPECT_TRUE(result.get());

  // Get again
  result = client.Get(doc, "key");
  EXPECT_TRUE(result.isSome());
  EXPECT_TRUE(result.get());
}

TEST_F(ClientTest, SetFailed) {
  vector<Cluster> clusters;
  clusters.push_back(Cluster("127.0.0.1", 4002));
  Client client(clusters);
  Document doc;
  Try<bool> result = client.Set(doc, "key", "hello");
  EXPECT_TRUE(result.isSome());
  EXPECT_TRUE(result.get() == false);
}

TEST_F(ClientTest, SetTTL) {
  Client client(clusters);
  SetOption option;
  option.SetTTL(5);

}

}


