AUTOMAKE_OPTIONS=no-dependencies
noinst_LIBRARIES=libetcdcpp.a
libetcdcpp_a_SOURCES = \
					  etcd.cpp

libetcdcpp_a_CPPFLAGS = \
					   -static \
					   -I../include \
					   -L/lib64 -lcurl \
					   -I../3rdparty/include
