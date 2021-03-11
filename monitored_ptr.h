#pragma once
#include <ctime>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>

namespace monitored_ptr
{
	class _Base_Monitored_Ptr;
	_Base_Monitored_Ptr* list_All_Objects = nullptr;  // 采用循环链表结构
	time_t inactive_time_tolerance;
	int thread_sleeping_seconds;
	std::mutex item_cdtor_lock;  // 是否正在构造或析构
	std::atomic_bool running{ true };

	class _Base_Monitored_Ptr
	{
	private:
		bool m_using;
		_Base_Monitored_Ptr* prev = nullptr;
		_Base_Monitored_Ptr* next = nullptr;
		
	
	protected:
		std::mutex calling_lock;
		std::mutex serializing_lock;
		std::mutex deserializing_lock;
		bool m_blank;
		volatile bool m_serialized;
		volatile int call_count;
		volatile time_t previous_call_time;
		volatile time_t call_time;
		void Do_Serialize()
		{
			std::lock_guard<std::mutex> lck(serializing_lock);
			calling_lock.lock();
			calling_lock.unlock();
			if (Serialize())
			{
				m_serialized = true;
			}
			else
			{
				throw std::exception("serializing failed");
			}

		}
		void Do_Deserialize()
		{
			std::lock_guard<std::mutex> lck(deserializing_lock);
			if (Deserialize())
			{
				call_count = 0;  // 这里重置call_count
				m_serialized = false;
			}
			else
			{
				throw std::exception("deserializing failed");
			}
		}

	public:
		static void thread_monitoring();
		_Base_Monitored_Ptr()
		{
			item_cdtor_lock.lock();
			call_count = 0;
			previous_call_time = call_time;
			call_time = time(nullptr);
			m_serialized = false;
			_Base_Monitored_Ptr* end = list_All_Objects->prev;
			prev = end;
			next = list_All_Objects;
			list_All_Objects->prev = this;
			end->next = this;
			m_using = true;
			item_cdtor_lock.unlock();
		}
		_Base_Monitored_Ptr(const bool create_first)
		{
			if (create_first)
			{
				m_serialized = false;
				m_using = false;
				prev = this;
				next = this;
			}
		}
		~_Base_Monitored_Ptr()
		{
			m_using = false;
			item_cdtor_lock.lock();
			if (m_serialized) Do_Deserialize();
			prev->next = next;
			next->prev = prev;
			item_cdtor_lock.unlock();
		}
		virtual bool Serialize()
		{
			return true;
		}
		virtual bool Deserialize()
		{
			return true;
		}
	};

	// 这个指针的行为比较像unique_ptr
	template <class _Ty>
	class monitored_ptr : private _Base_Monitored_Ptr
	{
	private:
		_Ty* m_object;

	public:
		_Ty* operator -> ()
		{
			std::lock_guard<std::mutex> lck(calling_lock);
			serializing_lock.lock();
			serializing_lock.unlock();
			if (m_serialized) Do_Deserialize();     // 必须先DeSerialize，不然可能文件名找不到，下同。
			call_count++;
			previous_call_time = call_time;
			call_time = time(nullptr);
			return m_object;
		}
		_Ty& operator * ()
		{
			std::lock_guard<std::mutex> lck(calling_lock);
			serializing_lock.lock();
			serializing_lock.unlock();
			if (m_serialized) Do_Deserialize();
			call_count++;
			previous_call_time = call_time;
			call_time = time(nullptr);
			return *m_object;
		}
		monitored_ptr& operator = (_Ty& new_obj)
		{
			std::lock_guard<std::mutex> lck(calling_lock);
			serializing_lock.lock();
			serializing_lock.unlock();
			if (m_serialized) Do_Deserialize();
			call_count++;
			previous_call_time = call_time;
			call_time = time(nullptr);
			delete m_object;
			m_object = &new_obj;
			if (m_blank) m_blank = false;
			return *this;
		}
		monitored_ptr& operator = (_Ty* new_obj)
		{
			std::lock_guard<std::mutex> lck(calling_lock);
			serializing_lock.lock();
			serializing_lock.unlock();
			if (m_serialized) Do_Deserialize();
			call_count++;
			previous_call_time = call_time;
			call_time = time(nullptr);
			delete m_object;
			m_object = new_obj;
			if (m_blank) m_blank = false;
			return *this;
		}
		monitored_ptr()
		{
			m_object = new _Ty;
			m_blank = true;
		}
		monitored_ptr(_Ty* ptr)
		{
			m_object = ptr;
			m_blank = false;
		}
		~monitored_ptr()
		{
			calling_lock.lock();
			calling_lock.unlock();
			if (m_serialized) Do_Deserialize();
			delete m_object;
		}
		bool Serialize() override
		{
			std::ostringstream filename;
			filename << m_object << "_" << call_count <<  "_" << call_time;
			yas::save<yas::file | yas::binary>(filename.str().c_str(), YAS_OBJECT("obj", *m_object));
			delete m_object;
			return true;
		}
		bool Deserialize() override
		{
			std::ostringstream filename;
			filename << m_object << "_" << call_count << "_" << call_time;
			m_object = new _Ty;
			yas::load<yas::file | yas::binary>(filename.str().c_str(), YAS_OBJECT("obj", *m_object));
			std::filesystem::remove(filename.str());
			return true;
		}
	};

	void _Base_Monitored_Ptr::thread_monitoring()
	{
		for (_Base_Monitored_Ptr* ptr = list_All_Objects; true; ptr = ptr->next)
		{
			if (ptr->m_blank) continue;
			if (!running) break;
			{  // 在一个单独的作用域中对当前对象的构造析构锁加锁
				std::lock_guard<std::mutex> incdtor(item_cdtor_lock);
				if (ptr == nullptr) ptr = list_All_Objects;
				if (!ptr->m_using) continue;
			}
			
			time_t now;
			time(&now);
			if ((now - ptr->call_time) > inactive_time_tolerance && !ptr->m_serialized && ptr->calling_lock.try_lock())
			{
				ptr->calling_lock.unlock();
				ptr->Do_Serialize();
			}

			std::this_thread::yield();
		}
		std::this_thread::sleep_for(std::chrono::seconds(thread_sleeping_seconds));
	}

	std::unique_ptr<std::thread> monitoring_thread;
	void Initialize(time_t tolerance = 5000, int sleep_seconds = 0)
	{
		list_All_Objects = new _Base_Monitored_Ptr(true);
		monitoring_thread = std::make_unique<std::thread>(_Base_Monitored_Ptr::thread_monitoring);
		inactive_time_tolerance = tolerance;
		thread_sleeping_seconds = sleep_seconds;
		std::cout << "自动序列化框架初始化完毕，tolerance=" << tolerance << ";sleep_seconds=" << sleep_seconds << std::endl;
	}

	void Finish()
	{
		running = false;
		monitoring_thread->join();
		delete list_All_Objects;
		std::cout << "自动序列化框架已停止" << std::endl;
	}
}