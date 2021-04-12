/*/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               创建：丁宋涛 夏曹俊，此代码可用作为学习参考                **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : C++Mysql8.0数据库跨平台编程实战
** Contact       : xiacaojun@qq.com
**  博客   : http://blog.csdn.net/jiedichina
**	视频课程 : 网易云课堂	http://study.163.com/u/xiacaojun
               腾讯课堂		https://jiedi.ke.qq.com/
               csdn学院		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961
**             51cto学院	http://edu.51cto.com/lecturer/index/user_id-12016059.html
** 			   老夏课堂		http://www.laoxiaketang.com
**
**   C++Mysql8.0数据库跨平台编程实战课程 课程群 ：735540120 加入群下载代码和交流
**   微信公众号  : jiedi2007
**		头条号	 : 夏曹俊
**
*****************************************************************************
//！！！！！！！！！ C++Mysql8.0数据库跨平台编程实战 课程  QQ群：735540120 下载代码和交流*/
#include "LXData.h"
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h> 
#else
#include <iconv.h>
#endif

using namespace std;
namespace LX {
#ifndef _WIN32
    static size_t Convert (char *from_cha, char *to_cha, char *in, size_t inlen, char *out, size_t outlen) {
        //转换上下文
        iconv_t cd;
        cd = iconv_open (to_cha, from_cha);
        if (cd == 0)
            return -1;
        memset (out, 0, outlen);
        char **pin = &in;
        char **pout = &out;
        //返回转换字节数的数量，但是转GBK时经常不正确 >=0就成功
        size_t re = iconv (cd, pin, &inlen, pout, &outlen);
        iconv_close (cd);
        return re;
    }
#endif



    LXData::LXData (const int *d) {
        this->type = LX_TYPE_LONG;
        this->data = (const char*) d;
        this->size = sizeof (int);
    }
    LXData::LXData (const char* data) {
        this->type = LX_TYPE_STRING;
        if (!data)return;
        this->data = data;
        this->size = strlen (data);
    }

    //读取文件，内容写入到data，size大小
    bool LXData::LoadFile (const char* filename) {
        if (!filename)return false;
        fstream in (filename, ios::in | ios::binary);
        if (!in.is_open ()) {
            cerr << "LoadFile " << filename << " failed!" << endl;
            return false;
        }
        //文件大小
        in.seekg (0, ios::end);
        size = in.tellg ();
        in.seekg (0, ios::beg);
        if (size <= 0) {
            return false;
        }
        data = new char[size];
        int readed = 0;
        while (!in.eof ()) {
            in.read ((char*) data + readed, size - readed);
            if (in.gcount () > 0)
                readed += in.gcount ();
            else
                break;
        }
        in.close ();
        this->type = LX_TYPE_BLOB;
        return true;
    }

    bool LXData::SaveFile (const char * filename) {
        if (!data || size <= 0)
            return false;
        fstream out (filename, ios::out|ios::binary);
        if (!out.is_open ()) {
            cout << "SaveFile failed!open failed!"<< filename << endl;
            return false;
        }
        out.write (data, size);
        out.close ();
        return true;
    }
    //释放LoadFile申请的data空间
    void LXData::Drop () {
        delete data;
        data = NULL;
    }

    std::string LXData::UTF8ToGBK () {
        string re = "";
        //1 UFT8 转为unicode win utf16
    #ifdef _WIN32
        //1.1 统计转换后字节数
        int len = MultiByteToWideChar (CP_UTF8,	//转换的格式
                                       0,			//默认的转换方式
                                       data,		//输入的字节
                                       -1,			//输入的字符串大小 -1 找\0
                                       0,//输出
                                       0//输出的空间大小
        );
        if (len <= 0)
            return re;
        wstring udata;
        udata.resize (len);
        MultiByteToWideChar (CP_UTF8, 0, data, -1, (wchar_t*) udata.data (), len);

        //2 unicode 转GBK
        len = WideCharToMultiByte (CP_ACP, 0, (wchar_t*) udata.data (), -1, 0, 0,
                                   0,	//失败默认替代字符
                                   0	//是否使用默认替代 
        );
        if (len <= 0)
            return re;
        re.resize (len);
        WideCharToMultiByte (CP_ACP, 0, (wchar_t*) udata.data (), -1, (char*) re.data (), len, 0, 0);
    #else
        re.resize (1024);
        int inlen = strlen (data);
        Convert ((char*)"utf-8", (char*)"gbk", (char*) data, inlen, (char*) re.data (), re.size ());
        int outlen = strlen (re.data ());
        re.resize (outlen);
    #endif
        return re;
    }
    std::string LXData::GBKToUTF8 () {
        string re = "";
    #ifdef _WIN32	
        //GBK转unicode
        //1.1 统计转换后字节数
        int len = MultiByteToWideChar (CP_ACP,	//转换的格式
                                       0,			//默认的转换方式
                                       data,		//输入的字节
                                       -1,			//输入的字符串大小 -1 找\0
                                       0,//输出
                                       0//输出的空间大小
        );
        if (len <= 0)
            return re;
        wstring udata;
        udata.resize (len);
        MultiByteToWideChar (CP_ACP, 0, data, -1, (wchar_t*) udata.data (), len);

        //2 unicode 转utf-8
        len = WideCharToMultiByte (CP_UTF8, 0, (wchar_t*) udata.data (), -1, 0, 0,
                                   0,	//失败默认替代字符
                                   0	//是否使用默认替代 
        );
        if (len <= 0)
            return re;
        re.resize (len);
        WideCharToMultiByte (CP_UTF8, 0, (wchar_t*) udata.data (), -1, (char*) re.data (), len, 0, 0);
    #else
        re.resize (1024);
        int inlen = strlen (data);
        Convert ((char*)"gbk", (char*)"utf-8", (char*) data, inlen, (char*) re.data (), re.size ());
        int outlen = strlen (re.data ());
        re.resize (outlen);
    #endif
        return re;
    }

}