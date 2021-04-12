
#include "XClient.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termio.h>
char _getch() {

    termios new_tm;
    //原来的模式 
    termios old_tm;
    int fd = 0;
    if (tcgetattr(fd, &old_tm) < 0)
        return -1;

    //更改为原始模式，没有回显
    cfmakeraw(&new_tm);
    if (tcsetattr(fd, TCSANOW, &new_tm) < 0) {
        return -1;
    }
    char c = getchar();
    if (tcsetattr(fd, TCSANOW, &old_tm) < 0) {
        return -1;
    }
    return c;
}

#endif
#include <chrono>
using namespace std;
using namespace chrono;
using namespace LX;

std::string XClient::InputPassword() {
    //清空缓冲
    cin.ignore(4096, '\n');
    string password = "";
    for (;;) {
        //获取输入字符不显示
        char a = _getch();
        if (a <= 0 || a == '\n' || a == '\r')
            break;
        cout << "*" << flush;
        password += a;
    }
    return password;
}
std::string XClient::Input() {
    string input = "";
    for (;;) {
        char a = getchar();
        if (a <= 0 || a == '\n' || a == '\r')
            break;
        input += a;
    }
    return input;
}

bool XClient::CheckInput(const std::string &in) {
    //不允许出现的字符
    string str = "'\"()";
    for (char a : str) {
        size_t found = in.find(a);
        if (found != string::npos) //发现违规字符
        {
            return false;
        }
    }
    return true;
}
//用户登录
bool XClient::Login() {
    bool isLogin = false;

    for (int i = 0; i < maxLoginTimes; i++) {
        string username = "";
        //接收用户输入
        cout << "input username:" << flush;
        cin >> username;
        cout << "[" << username << "]" << endl;


        //注入攻击
        // 如果你支持运行多条sql语句，结束你的语句，在添加自己的语句（删库）
        // 无密码直接登录
        // 限制用户的权限，不用root用户
        // 用预处理语句stmt
        // 检查用户的输入
        //select id from t_user where user='root' and pass=md5('123456')
        //select id from t_user where user='1'or'1'='1' and pass=md5('1')or'c4ca4238a0b923820dcc509a6f75849b'=md5('1')
        // username =  1'or'1'='1
        // password = 1')or'c4ca4238a0b923820dcc509a6f75849b'=md5('1
        //接收密码输入
        string password;
        cout << "input password:" << flush;
        password = InputPassword();

        if (!CheckInput(username) || !CheckInput(password)) {
            cout << "Injection attacks" << endl;
            continue;
        }

        string sql = "select id from t_user where user='";
        sql += username;
        sql += "' and pass=md5('";
        sql += password;
        sql += "')";
        cout << sql << endl;
        auto rows = my->GetResult(sql.c_str());
        if (rows.size() > 0) {
            cout << "login success！" << endl;
            isLogin = true;
            break;
        }
        cout << "login failed！" << endl;

        //cout << "[" << password << "]" << endl;
    }
    return isLogin;
}

//审计结果显示
void XClient::c_audit(std::vector<std::string>cmds) {
    //统计总数量
    string sql = "select count(*) from t_audit";
    auto rows = my->GetResult(sql.c_str());
    int total = 0;
    if (rows.size() > 0 || rows[0][0].data) {
        total = atoi(rows[0][0].data);
    }

    sql = "select * from t_audit";
    rows = my->GetResult(sql.c_str());
    //遍历每一行
    for (auto row : rows) {
        //遍历每一列
        for (auto c : row) {
            if (c.data) cout << c.data << " ";
        }
        cout << endl;
    }
    cout << "total:" << total << endl;
}

// test 10000 插入一万条测试数据
void XClient::c_test(std::vector<std::string>cmds) {
    int count = 10000;
    if (cmds.size() > 1)
        count = atoi(cmds[1].c_str());
    my->StartTransaction();
    for (int i = 0; i < count; i++) {
        XDATA data;
        stringstream ss;
        ss << "testlog";
        ss << (i + 1);
        string tmp = ss.str();
        data["log"] = tmp.c_str();
        data["ip"] = "127.0.0.1";

        //插入时间，用mysql now（）
        //@表示 字段内容不加引号，@会自动去除
        data["@log_time"] = "now()";
        my->Insert(data, "t_log");

    }

    {
        XDATA data;
        stringstream ss;
        ss << "search001";
        string tmp = ss.str();
        data["log"] = tmp.c_str();
        data["ip"] = "10.0.0.1";

        //插入时间，用mysql now（）
        //@表示 字段内容不加引号，@会自动去除
        data["@log_time"] = "now()";
        my->Insert(data, "t_log");

    }
    my->Commit();
    my->StopTransaction();

}
//like 搜索ip，和日志内容 like
void XClient::c_like(std::vector<std::string>cmds) {
    cout << "c_like" << endl;
    if (cmds.size() < 2)return;
    string key = cmds[1];
    //记录查询时间


    //记录开始时间
    auto start = system_clock::now();
    string sql = "select * from t_log ";
    string where = " where log like '%";
    where += key;
    where += "%'";
    sql += where;
    //一百万数据 无索引 0.47秒 有索引 0.000687
    auto rows = my->GetResult(sql.c_str());
    //遍历每一行
    for (auto row : rows) {
        //遍历每一列
        for (auto c : row) {
            if (c.data)
                cout << c.data << " ";
        }
        cout << endl;
    }

    //记录结束时间 -得出耗时
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start); //微秒
    cout << "time  sec =" << double(duration.count()) * microseconds::period::num / microseconds::period::den << " sec" << endl;

    //统计总数
    sql = "select count(*) from t_log ";
    sql += where;
    rows = my->GetResult(sql.c_str());

    int total = 0;
    if (rows.size() > 0 && rows[0][0].data)
        total = atoi(rows[0][0].data);
    cout << "total :" << total << endl;

}
//search 127.0.0.1 搜索ip
void XClient::c_search(std::vector<std::string>cmds) {
    cout << "c_search" << endl;
    if (cmds.size() < 2)return;
    string key = cmds[1];
    //记录查询时间


    //记录开始时间
    auto start = system_clock::now();
    string sql = "select * from t_log ";
    string where = " where ip='";
    where += key;
    where += "'";
    sql += where;
    //一百万数据 无索引 0.47秒 有索引 0.000687
    auto rows = my->GetResult(sql.c_str());
    //遍历每一行
    for (auto row : rows) {
        //遍历每一列
        for (auto c : row) {
            if (c.data)
                cout << c.data << " ";
        }
        cout << endl;
    }

    //记录结束时间 -得出耗时
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start); //微秒
    cout << "time  sec =" << double(duration.count()) * microseconds::period::num / microseconds::period::den << " sec" << endl;

    //统计总数
    sql = "select count(*) from t_log ";
    sql += where;
    rows = my->GetResult(sql.c_str());

    int total = 0;
    if (rows.size()>0 && rows[0][0].data)
        total = atoi(rows[0][0].data);
    cout << "total :" << total << endl;


}
////log 1 10 第一页，每页十条
void XClient::c_log(std::vector<std::string>cmds) {
    int pagecount = 10;
    int page = 1;
    if (cmds.size() > 1)
        page = atoi(cmds[1].c_str());
    if (cmds.size() > 2)
        pagecount = atoi(cmds[2].c_str());

    cout << "In log" << endl;
    // limit 0,10 从0开始取十条
    string sql = "select * from t_log limit ";
    stringstream ss;
    ss << sql;
    ss << (page - 1)*pagecount;
    ss << ",";
    ss << pagecount;
    sql = ss.str();
    auto rows = my->GetResult(sql.c_str());
    //遍历每一行
    for (auto row : rows) {
        //遍历每一列
        for (auto c : row) {
            if (c.data)
                cout << c.data << " ";
        }
        cout << endl;
    }
    cout << "Page = " << page << " PageCount = " << pagecount << endl;
}
void XClient::Main() {
    //用户登录
    if (!Login())return;

    ///分页显示 t_log
    //获取用户的输入
    for (;;) {
        cout << "Input:" << flush;
        string cmd = Input();
        cout << cmd << endl;
        //log 1 10 第一页 一页十行
        //切割空格
        vector<string> cmds;
        char *p = strtok((char*) cmd.c_str(), " ");
        while (p) {
            cmds.push_back(p);
            p = strtok(0, " ");
        }
        string type = cmd;
        if (cmds.size() > 0)
            type = cmds[0];

        if (type == "log") {
            c_log(cmds);
        } else if (type == "audit") {
            c_audit(cmds);
        } else if (type == "test") {
            c_test(cmds);
        } else if (type == "search") {
            c_search(cmds);
        } else if (type == "like") {
            c_like(cmds);
        }


    }



}
bool XClient::Init(std::string ip) {
    cout << "XClient::Init " << ip << endl;
    my = new LXMysql();
    if (!my->Connect(ip.c_str(), "root", "123", "hanxinle")) {
        cerr << "db connect faield!" << endl;
        return false;
    }
    cout << "db connect success!" << endl;
    return my->Query("set names utf8");
}
XClient::XClient() { }


XClient::~XClient() { }
