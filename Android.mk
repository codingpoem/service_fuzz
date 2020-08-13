LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := libutils libbinder libmedia libcutils libgui libui libdl liblog
LOCAL_MODULE_TAGS = optional

LOCAL_SRC_FILES:=  check_code.cpp
LOCAL_MODULE:= check_code
# LOCAL_SRC_FILES:=  test.cpp
# LOCAL_MODULE:= test2

LOCAL_CFLAGS += -Wno-unused-const-variable -Wno-unused-variable -Wno-unused-parameter
include $(BUILD_EXECUTABLE)


