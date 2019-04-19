#pragma once
#ifndef SharedBuf_H
#define SharedBuf_H
#include<queue>
#include<string>
#include <mutex>
#include <condition_variable>
#include <iostream>
class SharedBuf {
public:
	std::queue<std::string> q;
	std::mutex mtx;
	std::condition_variable not_full;
	std::condition_variable not_empty;
	unsigned int size;
	SharedBuf() {};
};
#endif
