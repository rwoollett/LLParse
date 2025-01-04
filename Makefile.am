ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(srcdir)

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = Scanner/scanner.cpp \
                        Scanner/scanner.h \
                        FileScan/FileScan.cpp \
                        FileScan/FileScan.h \
                        Common/io_utility.cpp \
                        Common/io_utility.h


libscanner_la_LDFLAGS = -version-info 1:0:1

bin_PROGRAMS = llparse
llparse_SOURCES = src/main.cpp \
                  src/main.h \
                  src/llparsetable.cpp \
                  src/llparsetable.h \
                  src/parsegrammar.cpp \
                  src/parsegrammar.h \
                   Utilities/Utilities.cpp \
                   Utilities/Utilities.h

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

