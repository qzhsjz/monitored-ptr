#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "monitored_ptr.h"
// #define _USE_UNIQUE_PTR
#define _USE_MONITORED_PTR
int main()
{
	system("pause");
#ifdef _USE_MONITORED_PTR
	monitored_ptr::Initialize(1, 0);
	monitored_ptr::monitored_ptr<std::string> all_objects[1000];
#endif
#ifdef _USE_UNIQUE_PTR
	std::unique_ptr<std::string> all_objects[1000];
#endif
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	for(int i = 0; i < 1000; i++)
	{
		std::ifstream file("hugestring.txt");
		std::ostringstream tmp;
		tmp << file.rdbuf();
#ifdef _USE_MONITORED_PTR
		all_objects[i] = new std::string(tmp.str());
#endif
#ifdef _USE_UNIQUE_PTR
		all_objects[i] = std::make_unique<std::string>(tmp.str());
#endif
		std::cout << i << " " << &*all_objects[i] << " 字符串长度：" << all_objects[i]->length() << std::endl;
	}
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	std::cout << "加载字符串用时：" << (end - start).count() << std::endl;
	for (int j = 0; j < 4; j++)
	{
		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::seconds(10));
		for (int i = 0; i < 1000; i++)
		{
			std::cout << i << " " << &*all_objects[i] << " 字符串长度：" << all_objects[i]->length() << std::endl;
		}
		std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
		std::cout << "加载字符串用时：" << (end - start).count() << std::endl;
	}
#ifdef _USE_MONITORED_PTR
	monitored_ptr::Finish();
#endif
	system("pause");
}