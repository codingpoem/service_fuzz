# function
通过binder进程间通讯的原理对手机系统服务进行安全性测试，查看是否可以通过发送异常数据引发系统服务的异常崩溃现象。从而发掘系统服务是否存在安全风险。
测试的系统服务必须在servicemanager注册过，也就是adb shell service list能够查找到。

# usage
把代码放入安卓aosp环境。利用mm进行模块化编译。把生成文件放入手机执行即可。

HWPCT:/data/local/tmp $ ./service_fuzz -h                                                                 
         -h,  --help           show help
         -s,  --service        fuzz a service
         -f,  --file           fuzz all services in file
         -a,  --all            fuzz all services in servicemanager
参数说明：
-s  对某个系统服务进行测试。
-f  对文件中列出的系统服务进行测试。
-a  测试所有servicemanager注册的系统服务

程序执行过程中会收集logcat日志，程序执行完毕后，当前目录会生成一个crashs.log文件，如果服务在处理相关数据出现了异常，这个文件记录了在测试过程中引发系统crash的相关日志。可以通过查看相关日志定位crash原因。


# example
130|HWPCT:/data/local/tmp $ ./service_fuzz -s hwPcManager                                     
service name:hwPcManager
interface name:android.pc.IHwPCManager
--crash!,code:25
HWPCT:/data/local/tmp $ 


HWPCT:/data/local/tmp $ cat crashs.log                                                                    
---------android.pc.IHwPCManager-------
08-13 19:18:53.623 10940 11836 E AndroidRuntime: *** FATAL EXCEPTION IN SYSTEM PROCESS: Binder:10940_D
08-13 19:18:53.623 10940 11836 E AndroidRuntime: java.lang.OutOfMemoryError: Failed to allocate a 3954027824 byte allocation with 25165824 free bytes and 477MB until OOM, target footprint 60913120, growth limit 536870912
08-13 19:18:53.623 10940 11836 E AndroidRuntime: 	at java.util.ArrayList.<init>(ArrayList.java:166)
08-13 19:18:53.623 10940 11836 E AndroidRuntime: 	at android.os.Parcel.createTypedArrayList(Parcel.java:2539)
08-13 19:18:53.623 10940 11836 E AndroidRuntime: 	at android.pc.IHwPCManager$Stub.onTransact(IHwPCManager.java:739)
08-13 19:18:53.623 10940 11836 E AndroidRuntime: 	at android.os.Binder.execTransactInternal(Binder.java:1028)
08-13 19:18:53.623 10940 11836 E AndroidRuntime: 	at android.os.Binder.execTransact(Binder.java:1001)
HWPCT:/data/local/tmp $ 


说明：对系统服务hwPcManager进行测试，测试的服务名称为hwPcManager，对应的系统服务接口是android.pc.IHwPCManager，引起crash的binder通信code是25。
