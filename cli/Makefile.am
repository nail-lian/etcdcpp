AUTOMAKE_OPTIONS=no-dependencies
noinst_PROGRAMS = etcd-cli
etcd_cli_SOURCES = main.cpp

etcd_cli_LDADD = \
					  ../src/libetcdcpp.a \
					-ldl -lz  -lrt

AM_CPPFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/3rdparty/include
etcd_cli_CPPFLAGS = \
					  -Wall \
					  -lpthread \
					  -I$(top_srcdir)/include \
					  -I$(top_srcdir)/3rdparty/include

etcd_cli_LDFLAGS =  \
						 -I$(top_srcdir)/include \
						 -I$(top_srcdir)/3rdparty/include
