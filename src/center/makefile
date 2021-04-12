center:center.cpp XCenter.cpp XCenter.h
	g++ $^ -o $@ -I ../LXMysql -lLXMysql
	#./$@ install 127.0.0.1
	#./$@ add 192.168.0.202 fileserver1
	#./$@ add 192.168.0.201 fileserver2
	./$@
clean:
	rm -rf *.o
	rm -rf center
