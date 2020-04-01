#pragma once

#include "gameObject.h"

template <class T> class ObjectPool
{

    std::vector<T*> pool;

public:

	void create(unsigned int numObjects) {
        deallocate();
		for (unsigned int i = 0; i < numObjects; i++)
		{
			T* t = new T();
			pool.push_back(t);
		}
	}

	void destroy() {
		for (auto it = pool.begin(); it != pool.end(); it++)
			(*it)->destroy();
	}

	void deallocate() {
		for (auto it = pool.begin(); it != pool.end(); it++)
			delete *it;
	}

	~ObjectPool() {
        deallocate();
	}

	T* firstAvailable() {
		for (auto it = pool.begin(); it != pool.end(); it++)
			if (!(**it).enabled)
				return (*it);

		// if it reaches this point, there is no available object in the pool
		return NULL;
	}

	// select a random, enabled element in the object pool
	T* selectRandom() {
		int offset = rand() / (float) RAND_MAX * pool.size();

		for (int i = 0; i < pool.size(); i++)
		{
			int index = (i + offset) % pool.size();
			if (pool[index]->enabled)
				return pool[index];
		}
		return NULL;
	}

    void add(T *pObject) {
        pool.push_back(pObject);
    }

    std::vector<T*> getPool() {
        return pool;
    }
};