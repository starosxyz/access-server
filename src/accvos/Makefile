
include $(USER_DEPTH)/make/makeinclude/mk_pre.mak

USER_LIB = vosthread

USER_SRCS_CPP= \
CDIPCVOSThread.cpp

USER_INCLDIRS =\
-I$(USER_DEPTH)/src/\
-I$(USER_DEPTH)/src/main/\
-I$(USER_DEPTH)/src/dipcvnet/\
-I$(USER_DEPTH)/src/include/\
-I$(USER_DEPTH)/src/dipcvos/\
-I$(USER_DEPTH)/src/utils\
-I$(USER_DEPTH)/src/vswitch\
-I/opt/staros.xyz/vos/include/dpdkwrapper-fwd/\
-I/opt/staros.xyz/voskern-fwd/include/
USER_DEFFLAGS =
USER_LINK_LIBS =

USER_LINK_DLLS =\
-ldwstack -L/opt/staros.xyz/vos/libs
USER_LINK_FLAGS=\
-L/opt/staros.xyz/vos/libs



USER_OBJ_DIR=$(TARGET_PATH)/src/vnet

include $(USER_DEPTH)/make/makeinclude/mk_post.mak



