/*************************************************************************
*  Test program for multithreaded library usage.
* ------------------------------------------------------------------------
*  Copyright (c) 2022 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "../../../include/saim.h" // main library header

#include <cstdlib>
#include <cstdio>
#include <thread>
#include <future>
#include <memory>
#include <vector>
#include <string>

class ThreadInfo {
public:
	ThreadInfo(int id)
	: thread_id_(id)
	{
	}
	~ThreadInfo()
	{
		Destroy();
	}

	int id() { return thread_id_; }

	bool Create(int target_width, int target_height, int target_bpp)
	{
		unsigned char* buffer = new unsigned char[target_width * target_height * target_bpp];
		if (buffer == nullptr)
			return false;
		buffer_.reset(buffer);
		return saim_target_info__create(&target_info_, 
			buffer, target_width, target_height, target_bpp);
	}
	void Destroy()
	{
		saim_target_info__destroy(&target_info_);
		buffer_.reset(nullptr);
	}
	void RenderAligned(struct saim_instance * instance,
		double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
	{
		int num_tiles_left;
		do
		{
			num_tiles_left = saim_render_aligned_async(instance, &target_info_, 
				upper_latitude, left_longitude, lower_latitude, right_longitude);
			std::this_thread::yield();
		}
		while (num_tiles_left > 0);
	}
	size_t GetHash()
	{
		size_t buffer_size = target_info_.target_width * target_info_.target_height * target_info_.target_bpp;
		std::string string;
		string.resize(buffer_size + 1);
		memcpy(&string[0], buffer_.get(), buffer_size);
		string[buffer_size] = '\0';
		std::hash<std::string> hasher;
		size_t hash = hasher(string);
		return hash;
	}
private:
	int thread_id_;
	saim_target_info target_info_;
	std::unique_ptr<unsigned char> buffer_;
};

class ThreadPool {
public:
	ThreadPool(int target_width, int target_height, int target_bpp)
	: id_counter_(0)
	, target_width_(target_width)
	, target_height_(target_height)
	, target_bpp_(target_bpp)
	{
	}
	~ThreadPool() = default;

	bool MakeThreads()
	{
		for (size_t i = 0; i < kNumThreads; ++i)
			if (!MakeThreadInfo())
				return false;
		return true;
	}
	void RunThreads(struct saim_instance * instance,
		double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
	{
		threads_.resize(thread_infos_.size());
		for (size_t i = 0; i < kNumThreads; ++i)
		{
			auto& thread_info = thread_infos_[i];
			std::thread& thread = threads_[i];
			thread = std::thread(&ThreadInfo::RenderAligned, thread_info.get(), instance,
				upper_latitude, left_longitude, lower_latitude, right_longitude);
		}
	}
	void WaitForThreads()
	{
		for (auto& thread : threads_)
			thread.join();
	}
	void PrintHashes()
	{
		for (auto& thread_info : thread_infos_)
		{
			size_t hash = thread_info->GetHash();
			printf("Thread #%i has hash %zu\n", thread_info->id(), hash);
		}
	}
	void Release()
	{
		for (auto& thread_info : thread_infos_)
		{
			thread_info->Destroy();
		}
	}
protected:
	bool MakeThreadInfo()
	{
		std::unique_ptr<ThreadInfo> thread_info(new ThreadInfo(id_counter_++));
		if (!thread_info->Create(target_width_, target_height_, target_bpp_))
			return false;
		thread_infos_.push_back(std::move(thread_info));
		return true;
	}
private:
	static constexpr size_t kNumThreads = 4;
	int id_counter_;
	int target_width_;
	int target_height_;
	int target_bpp_;
	std::vector< std::unique_ptr<ThreadInfo> > thread_infos_;
	std::vector< std::thread > threads_;
};

int main()
{
	int result;
	saim_provider_info * info = 0;
	int flags = 0;
	int width, height, bytes_per_pixel;
	double upper_latitude, left_longitude, lower_latitude, right_longitude;
	struct saim_instance * instance;

	// Initialize libsaim
	instance = saim_init("", info, flags, 1, &result);
	if (result != 0)
		return result;

	// Buffer parameters per thread
	width = 640;
	height = 480;
	bytes_per_pixel = 3;

	// Setup thread pool
	ThreadPool pool(width, height, bytes_per_pixel);
	if (pool.MakeThreads())
	{
		// Now we are able to render
		upper_latitude = 34.890285;
		lower_latitude = 31.816744;
		left_longitude = -118.596199;
		right_longitude = -114.489501;
		pool.RunThreads(instance, upper_latitude, left_longitude, lower_latitude, right_longitude);

		// Wait for concurrent threads to complete
		pool.WaitForThreads();

		// Compare result with hashes:
		pool.PrintHashes();

		// Finally release thread info objects
		pool.Release();
	}

	// Cleanup libsaim
	saim_cleanup(instance);

	return 0;
}