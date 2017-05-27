template <class T>
class queue: private std::deque<T>
{
    public:
        queue()
		{
        }

        queue(const queue& pQueue)
        {
            while (notEmpty())
            {
                pop();
            }
            for (T item : pQueue)
            {
                push(item);
            }
        }

        void push(T pItem)
		{
            logs::debug << LOC() << "Pushing" << logs::done;
            std::unique_lock<std::mutex> lock(mMutex);
            std::deque<T>::push_back(pItem);
            logs::debug << LOC() << "Notifying " << &mIsEmpty << logs::done;
            mIsEmpty.notify_one();
        }

		T pop()
        {
			std::unique_lock<std::mutex> lock(mMutex);
			while(std::deque<T>::empty())
			{
                logs::debug << LOC() << "Waiting on " << &mIsEmpty << logs::done;
				mIsEmpty.wait(lock);
			}
			T value = std::move(std::deque<T>::front());
			std::deque<T>::pop_front();
			return value;
		}

        bool notEmpty()
        {
            return !std::deque<T>::empty();
        }

        bool empty()
        {
            return std::deque<T>::empty();
        }

        auto begin() const
        {
            return std::deque<T>::begin();
        }

        auto end() const
        {
            return std::deque<T>::end();
        }

        T operator[](int i) const
        {
            return std::deque<T>::operator[](i);
        }

        queue operator=(const queue& pQueue)
        {
            if (&pQueue != this)
            {
                std::deque<T>::operator=(pQueue);
            }
            return *this;
        }

    private:
        std::mutex mMutex;
        std::condition_variable mIsEmpty;
};
