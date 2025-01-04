ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(srcdir)

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = ../LLScanner/Scanner/scanner.cpp \
                        ../LLScanner/Scanner/scanner.h \
                        ../LLScanner/FileScan/FileScan.cpp \
                        ../LLScanner/FileScan/FileScan.h \
                        ../LLScanner/Common/io_utility.cpp \
                        ../LLScanner/Common/io_utility.h


libscanner_la_LDFLAGS = -version-info 1:0:1

bin_PROGRAMS = llparse
llparse_SOURCES = src/main.cpp \
                  src/main.h \
                  src/llparsetable.cpp \
                  src/llparsetable.h \
                  src/parsegrammar.cpp \
                  src/parsegrammar.h \
                   ../LLScanner/Utilities/Utilities.cpp \
                   ../LLScanner/Utilities/Utilities.h

# testprog_LDFLAGS = $(CPPUTEST_LIBS)
llparse_LDADD =  libscanner.la $(EXPAT_LIBS)

# TESTS = testprog

# bin_PROGRAMS = testprog
# testprog_SOURCES = cpputest/main.cpp \
#                    cpputest/ScannerTest.cpp \
#                    cpputest/ParseDateTest.cpp \
#                    cpputest/DateItemTest.cpp \
#                    Generate/filegenerate.cpp \
#                    Generate/filegenerate.h \
#                    Utilities/Utilities.cpp \
#                    Utilities/Utilities.h

# testprog_LDFLAGS = $(CPPUTEST_LIBS)
# testprog_LDADD =  libdatemodule.la $(EXPAT_LIBS)

# TESTS = testprog

