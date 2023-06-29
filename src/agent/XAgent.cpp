#define LOGPATH "auth.log"
#include "XAgent.h"
#include <iostream>
#include <thread>
using namespace LX;
using namespace std;
//agent 模块初始化，只能调用一次，在所有接口之前调用
bool XAgent::Init (std::string ip) {
    if (ip.empty ()) {
        cerr << "XAgent::Init failure.ip is empty" << endl;
        return false;
    }

    my = new LXMysql ();
    if (!my->Connect (ip.c_str (), "root", "123", "hanxinle")) {
        cerr << "mysql connect failure" << endl;
        return false;
    }

    fp = fopen (LOGPATH, "rb");
    if (!fp) {
        cerr << "open log " << LOGPATH << " failure" << endl;
        return false;
    }
    cout << "open log " << LOGPATH << " success." << endl;
    // 只审计系统开始运行之后的事件


    //文件移动到结尾
    fseek (fp, 0, SEEK_END);


    ////创建t_log日志表
    //string sql = "CREATE TABLE IF NOT EXISTS `t_log` (\
    	//			`id` INT AUTO_INCREMENT,\
	//			`ip` VARCHAR(16),\
	//			`log` VARCHAR(2048) ,\
	//		    `log_time` datetime,\
	//			PRIMARY KEY(`id`))";
    //my->Query(sql.c_str());

    return true;
}
bool  XAgent::SaveLog (std::string log) {
    cout << log << endl;
    XDATA data;
    data["log"] = log.c_str ();
    data["ip"] = local_ip.c_str ();

    //插入时间，用mysql now（）
    //@表示 字段内容不加引号，@会自动去除
    data["@log_time"] = "now()";
    my->Insert (data, "t_log");
    return true;
    return true;
}
void XAgent::Main () {
    // 读取最新的日志
    string log = "";
    for (;;) {

        char c = fgetc (fp);
        // 没有字符串或者读到文件结尾
        if (c <=0) {
            this_thread::sleep_for (100ms);
            continue;
        }
        if (c == '\n') {
            cout << log << endl;
            log = "";
            continue;
        }
        log += c;
        cout << log<< flush;
    }

}

std::string XAgent::GetLocalIp () {
    char ip[16] = {0};
#ifndef _WIN32
    ifaddrs *ifadd = 0;
    if (getifaddrs (&ifadd) != 0)return "";
    //遍历地址
    ifaddrs *iter = ifadd;

    while (iter != NULL) {
        //ipv4
        if (iter->ifa_addr->sa_family == AF_INET)
            if (strcmp (iter->ifa_name, "lo")!= 0) //去掉回环地址 127.0.0.1
            {
                //转换整形ip为字符串

                void *tmp = &((sockaddr_in*) iter->ifa_addr)->sin_addr;
                inet_ntop (AF_INET, tmp, ip, INET_ADDRSTRLEN);
                break;
            }
        iter = iter->ifa_next;
    }
    freeifaddrs (ifadd);
#endif
    return ip;

}


XAgent::XAgent () {

}

XAgent::~XAgent () {

}
