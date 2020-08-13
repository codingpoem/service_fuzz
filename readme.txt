# function
通过binder进程间通讯的原理对手机系统服务进行安全性测试，查看是否可以通过发送异常数据引发系统服务的异常崩溃现象。
测试的系统服务必须在servicemanager注册过，也就是adb shell service list能够查找到。

# usage
把代码放入安卓aosp环境。利用mm进行模块化编译。把生成文件放入手机执行即可。

HWPCT:/data/local/tmp $ ./service_fuzz -h                                                                 
         -h,  --help           short help
         -s,  --service        fuzz a service
         -f,  --file           fuzz all services in file
         -a,  --all            fuzz all services in servicemanager
参数说明：
-s  测试某个系统服务。
-f  测试来自文件中的系统服务。
-a  测试所有servicemanager注册的系统服务

程序执行过程中会手机logcat日志，执行完毕当前目录会生成一个crashs.log文件，这个文件记录了在测试过程中引发系统crash的相关日志。可以通过查看相关日志定位crash原因。


# example
HWPCT:/data/local/tmp $ ./service_fuzz -s alarm                                                           
service_name:alarm
interface name:android.app.IAlarmManager
--crash service name:android.app.IAlarmManager
HWPCT:/data/local/tmp $ cat crashs.log                                                                    
---------android.app.IAlarmManager-------
08-13 19:14:39.544 30040 30891 E AndroidRuntime: *** FATAL EXCEPTION IN SYSTEM PROCESS: Binder:30040_E
08-13 19:14:39.544 30040 30891 E AndroidRuntime: java.lang.OutOfMemoryError: Failed to allocate a 1268389880 byte allocation with 25165824 free bytes and 476MB until OOM, target footprint 62467936, growth limit 536870912
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.os.Parcel.createStringArray(Parcel.java:1367)
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.os.WorkSource.<init>(WorkSource.java:114)
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.os.WorkSource$1.createFromParcel(WorkSource.java:1182)
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.os.WorkSource$1.createFromParcel(WorkSource.java:1180)
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.app.IAlarmManager$Stub.onTransact(IAlarmManager.java:165)
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.os.Binder.execTransactInternal(Binder.java:1028)
08-13 19:14:39.544 30040 30891 E AndroidRuntime: 	at android.os.Binder.execTransact(Binder.java:1001)
HWPCT:/data/local/tmp $ 


