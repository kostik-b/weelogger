# Copyright QUB 2017

.PHONY : clean uninstall


SOURCE_DIR    := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

GENERIC_OPTS  := -I$(SOURCE_DIR)/ -g -O3 -fPIC -D_GNU_SOURCE 

C_OPTS        := $(GENERIC_OPTS)  -std=c11
CXX_OPTS      := $(GENERIC_OPTS)

all : wl_logger_lib

C_SRCS=wl_core.c wl_basic_attributes.c wl_stdout_sink.c wl_file_sink.c
CPP_SRCS=WeeLogger.cpp


# ---------------- C -----------------------------

define depend_macro_c

%.o.d: $(1)
	@set -e; rm -f $$@; \
	$$(CC) -MM $$(CFLAGS) $$(C_OPTS) $$< > $$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@; \
	rm -f $$@.$$$$

endef

$(eval $(call depend_macro_c,   $(SOURCE_DIR)/%.c))
$(eval $(call depend_macro_c,   $(SOURCE_DIR)/core/%.c))
$(eval $(call depend_macro_c,   $(SOURCE_DIR)/sinks/%.c))
$(eval $(call depend_macro_c,   $(SOURCE_DIR)/attributes/%.c))
$(eval $(call depend_macro_c,   $(SOURCE_DIR)/apps/%.c))

# ---------------- C++ -----------------------------

define depend_macro_cpp

%.o.d: $(1)
	@set -e; rm -f $$@; \
	$$(CXX) -MM $$(CXXFLAGS) $$(CXX_OPTS) $$< > $$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@; \
	rm -f $$@.$$$$

endef

$(eval $(call depend_macro_cpp,   $(SOURCE_DIR)/apps/%.cpp))
$(eval $(call depend_macro_cpp,   $(SOURCE_DIR)/cpp/%.cpp))
# ------- end dependencies


-include $(C_SRCS:%.c=%.o.d)
-include $(CPP_SRCS:%.cpp=%.o.d)

# a macro for the build pattern rules
define build_macro_c

%.o : $(1)
	$${CC} $$< -o $$@ -c $${CFLAGS} $${C_OPTS}

endef

$(eval $(call build_macro_c,   $(SOURCE_DIR)/%.c))
$(eval $(call build_macro_c,   $(SOURCE_DIR)/core/%.c))
$(eval $(call build_macro_c,   $(SOURCE_DIR)/sinks/%.c))
$(eval $(call build_macro_c,   $(SOURCE_DIR)/attributes/%.c))
$(eval $(call build_macro_c,   $(SOURCE_DIR)/apps/%.c))

define build_macro_cpp

%.o : $(1)
	$${CXX} $$< -o $$@ -c $${CXXFLAGS} $${CXX_OPTS} -D__FILE_NAME_ONLY__="\"$$(@:%.o=%) \""

endef

$(eval $(call build_macro_cpp,   $(SOURCE_DIR)/apps/%.cpp))
$(eval $(call build_macro_cpp,   $(SOURCE_DIR)/cpp/%.cpp))

# ------- end build macros --------------------

WEE_LOGGER_OBJS=$(C_SRCS:%.c=%.o) $(CPP_SRCS:%.cpp=%.o) 

wl_logger_objs : $(WEE_LOGGER_OBJS)
	echo "$^" > .wlg.objs

wl_logger_lib: $(WEE_LOGGER_OBJS)
	ar rcs libweelogger.a $(WEE_LOGGER_OBJS)
	$(CXX) $(CFLAGS) $^ -shared -fPIC -o libweelogger.so

basic_test : $(WEE_LOGGER_OBJS) basic_test.o
	$(CXX) $(CFLAGS) $^ -o $@

file_test : $(WEE_LOGGER_OBJS) file_test.o
	$(CXX) $(CFLAGS) $^ -o $@

# TODO: need to implement the proper C++ test
cpp_test : $(WEE_LOGGER_OBJS) cpp_test.o
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	- rm $(C_SRCS:%.c=%.o.d) $(C_SRCS:%.c=%.o) $(CPP_SRCS:%.cpp=%.o.d) $(CPP_SRCS:%.cpp=%.o) \
                          libweelogger.a libweelogger.so basic_test file_test cpp_test file_test.o \
                          basic_test.o cpp_test.o



