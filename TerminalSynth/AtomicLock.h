#pragma once

#include <atomic>

/// <summary>
/// Class to provide std::atomic synchronization
/// </summary>
class AtomicLock
{
public:

	AtomicLock() { }

	void AcquireLock()
	{
		// https://thelinuxcode.com/cpp-std-atomic/
		while (_lock.test_and_set()) {}
	}

	void Release()
	{
		// https://thelinuxcode.com/cpp-std-atomic/
		_lock.clear();
	}

private:

	/// <summary>
	/// std::atomic lock object! Use this for synchronizing wait loops for lock-less synchronization!
	/// </summary>
	std::atomic_flag _lock = ATOMIC_FLAG_INIT;
};