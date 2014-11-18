AUTOMAKE_OPTIONS=no-dependencies
noinst_LIBRARIES=libetcdcpp.a
libetcdcpp_a_SOURCES = \
					  etcd.cpp

libetcdcpp_a_CPPFLAGS = \
					   -static \
					   -L/usr/local/lib -lcurl \
					   -I../include \
					   -I../3rdparty/include

libetcdcpp_a_LIBADD = \
					   -L/usr/local/lib -lcurl
