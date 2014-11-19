check_PROGRAMS = tests
tests_SOURCES = \
				./client.cpp \
				./main.cpp

tests_CPPFLAGS = \
				 -Wall \
				 -UHANVE_CONFIG_H \
				 -I../include \
				 -I../3rdparty/include \
				 -ldl -lz

tests_LDADD = \
			  ../src/libetcdcpp.a \
			  ../3rdparty/lib/libgmock.a \
			  ../3rdparty/lib/libgtest.a \
			  -ldl -lz -lrt -lcurl

AM_CPPFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/3rdparty/include -I$(top_srcdir)/thrift/gen-cpp
tests_LDFLAGS = -lpthread \
				-I$(top_srcdir)/include \
				-I$(top_srcdir)/3rdparty/include \
				-I$(top_srcdir)/thrift/gen-cpp
