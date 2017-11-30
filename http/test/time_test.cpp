#include <iostream>

#include <sys/time.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>
int main()
{
    using namespace std;
    const int kMicroSecondsPerSecond = 1000000;

    time_t curtime = time(NULL);
    struct tm tartm = *localtime(&curtime);
    tartm.tm_hour = 23;
    tartm.tm_min = 39;
    tartm.tm_sec = 0;

    cout << asctime(&tartm) << endl;
    int64_t microSeconds = static_cast<int64_t>(mktime(&tartm) * kMicroSecondsPerSecond);

    time_t t = static_cast<time_t>(microSeconds / kMicroSecondsPerSecond);
    cout << ctime(&t) << endl;

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    cout << ctime(&tv.tv_sec) << endl;
    int64_t now = tv.tv_sec * kMicroSecondsPerSecond + static_cast<long>(tv.tv_usec / 1000);


    cout << (microSeconds - now) / kMicroSecondsPerSecond << endl;

    /* time_t curTime = time(NULL); */
    /* struct tm t = *localtime(&curTime); */
    /* cout << t.tm_year << " " << t.tm_mon << endl; */
    /* cout << t.tm_mday << " " << t.tm_wday << " " << t.tm_yday << endl; */
    /* cout << t.tm_hour << " " << t.tm_min << " " << t.tm_hour << endl; */
    /* cout << mktime(&t) << endl; */

    /* struct tm tarT = t; */
    /* tarT.tm_mday += 1; */
    /* tarT.tm_hour = 3; */
    /* tarT.tm_min = 0; */
    /* tarT.tm_sec = 0; */

    /* time_t diff = difftime(mktime(&tarT), mktime(&t)); */
    /* cout << diff << endl; */
    return 0;
}
