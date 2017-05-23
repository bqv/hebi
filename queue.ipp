template <class T> class queue: private std::deque<T>
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
            std::unique_lock<std::mutex> wlck(mWriterMutex);
            std::deque<T>::push_back(pItem);
            mIsEmpty.notify_all();
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

		T pop()
        {
			std::unique_lock<std::mutex> rlck(mReaderMutex);
			while(std::deque<T>::empty())
			{
				mIsEmpty.wait(rlck);
			}
			T value = std::deque<T>::front();
			std::deque<T>::pop_front();
			return value;
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
        std::mutex mReaderMutex;
        std::mutex mWriterMutex;
        std::condition_variable mIsEmpty;
};
