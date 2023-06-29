#include <string>
#include "LXMysql.h"
#include "LXData.h"

class XAgent {
public:
    static XAgent* Get () {
        static XAgent a;
        return &a;
    }
    //agent 模块初始化，只能调用一次，在所有接口之前调用
    bool Init (std::string ip);
    // 读取最新日志
    void Main ();
    //日志写入数据库
    bool SaveLog (std::string log);
    //获取本机的IP地址
    static std::string GetLocalIp ();
    ~XAgent ();
private:
    XAgent ();
    //数据库对象
    LX::LXMysql *my = nullptr;

    FILE * fp = 0;

    std::string local_ip;
};

