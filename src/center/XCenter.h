#include <string>
#include "LXMysql.h"
class XCenter {
public:
    bool Install(std::string ip);
    static XCenter *Get() {
        static XCenter xc;
        return &xc;
    }
    // 初始化数据库
    bool Init();

    //添加设备
    bool AddDevice(std::string ip, std::string name);

    void Main();
    ~XCenter();
private:
    XCenter();

    LX::LXMysql * my = nullptr;
};

