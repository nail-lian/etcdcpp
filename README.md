# Etcdcpp
etcd api for cpp

## Dependency

* rapidjson: https://github.com/miloyip/rapidjson
* libcurl: 
* stl:

## Usage
* install libcurl
* copye etcd.h and etcd.cpp and rapidjson headers into your project

## Example
‘’’cpp
#include <iostream>
int main() {
  return 0;
}
‘’’

## Run test
* dependency: autoconf >= 2.69, automake >= 1.14, gtest and gmock
* autoreconf
* ./configure
* ./make check
* ./tests/tests

## TODO
* Implement CAS API
* Write unittest for rest api

## Other
* developed in ubuntu 14.04
