#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <android/log.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace android;
using namespace std;

static const char *short_options = "hs::f::a";

static const struct option long_options[] = {
    {"verbose", optional_argument, NULL, 's'},
    {"verbose", optional_argument, NULL, 'f'},
    {"verbose", no_argument, NULL, 'a'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

void usage()
{
    printf("         -h,  --help           show help\n");
    printf("         -s,  --service        fuzz a service\n");
    printf("         -f,  --file           fuzz all services in file\n");
    printf("         -a,  --all            fuzz all services in servicemanager\n");
}

int check_crash()
{
    system("logcat -b crash -d > crash.log ");

    FILE *fp;
    char ch;
    if ((fp = fopen("crash.log", "r")) == NULL)
    {
        printf("Error!\n");
        exit(0);
    }
    ch = fgetc(fp);
    if (ch > -1 && ch < 128)
    {
        return 1;
        fclose(fp);
    }
    else
    {
        return 0;
        fclose(fp);
    }
}

void fuzz_service(sp<IBinder> service)
{
    String16 ifName = service->getInterfaceDescriptor();
    printf("interface name:%s\n", String8(ifName).string());
    for (uint32_t code = 0; code <= 50; code++)
    {
        Parcel data, reply;
        if (ifName.size() > 0)
            data.writeInterfaceToken(ifName);
        for (uint32_t i = 0; i < random() % 800; i++)
        {
            data.writeInt32(random());
        }
        service->transact(code, data, &reply, 1);
        sleep(1);
        if (check_crash())
        {
            printf("--crash!,code:%d\n", code);
            // system("echo ------------------------------------- >> crashs.log");
            string str1("echo ---------");
            str1.append(String8(ifName).string());
            str1.append("------- >> crashs.log");
            system(str1.c_str());
            system("cat crash.log >> crashs.log");
            sleep(10);
            break;
        }
    }
}

void fuzz_one(char *service_name)
{
    //clear crash log
    system("logcat -c");
    system("rm crash*");

    printf("service name:%s\n", service_name);

    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> service = sm->checkService(String16(service_name));
    if (service != NULL)
    {
        system("rm crash.log 2>/dev/null");
        fuzz_service(service);
    }
    else
    {
        printf("error:%s service binder is NULL\n", service_name);
    }
}

void fuzz_file(char *filePath)
{
    //clear crash log
    system("logcat -c");
    system("rm crash*");

    // printf("filePath:%s\n", filePath);

    // sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> service = NULL;

    ifstream file;
    file.open(filePath, ios::in);

    if (!file.is_open())

        exit(0);

    std::string strLine;
    while (getline(file, strLine))
    {
        if (strLine.empty())
            continue;
        // char *service_name = strLine.c_str();
        cout << "service name:" << strLine << endl;
        sp<IServiceManager> sm = defaultServiceManager();

        // Vector<String16> services = sm->listServices();
        // for (uint32_t i = 0; i < services.size(); i++)
        // {
        //     printf("----service name:%s\n", String8(services[i]).string());
        // }

        sp<IBinder> service = sm->checkService(String16(strLine.c_str()));

        if (service != NULL)
        {
            system("rm crash.log 2>/dev/null");
            system("logcat -c");
            fuzz_service(service);
        }
        else
        {
            printf("error:%s service binder is NULL\n", strLine.c_str());
        }
        // sleep(10);
    }
}

void fuzz_all()
{
    //clear crash log
    cout << "start fuzz all service in servicemanager" << endl;
    system("logcat -c");
    system("rm crash*");

    sp<IServiceManager> sm = defaultServiceManager();
    Vector<String16> services = sm->listServices();
    for (uint32_t i = 0; i < services.size(); i++)
    {
        String16 name = services[i];
        // char *service_name = String8(name).string();
        sp<IBinder> service = sm->checkService(name);
        printf("service name:%s\n", String8(name).string());

        if (service != NULL)
        {
            system("rm crash.log 2>/dev/null");
            system("logcat -c");
            fuzz_service(service);
        }
        else
        {
            printf("error:%s service binder is NULL\n", String8(name).string());
        }
    }
}

int main(int argc, char *argv[])
{
    int opt = 0;

    // while ((opt = getopt(argc, argv, string)) != -1)
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {

        {
            switch (opt)
            {
            case 'h':
                usage();
                break;
            case 's':
                char *service_name;
                if (optarg == NULL)
                {
                    service_name = argv[optind];
                }
                else
                {
                    service_name = optarg;
                }
                fuzz_one(service_name);
                break;
            case 'f':
                char *filePath;
                if (optarg == NULL)
                {
                    filePath = argv[optind];
                }
                else
                {
                    filePath = optarg;
                }
                fuzz_file(filePath);
                break;
            case 'a':
                fuzz_all();
                break;

            default:
                fuzz_all();
                break;
            }
        }
    }
}