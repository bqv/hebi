template <class T>
class queue: private std::deque<T>
{
    public:
        queue()
		{
            mMutex = std::make_shared<std::mutex>();
            mIsEmpty = std::make_shared<std::condition_variable>();
        }

        queue(const queue& pQueue)
            : std::deque<T>(pQueue)
        {
        }

        void push(T pItem)
		{
            //logs::debug << LOC() << "Pushing" << logs::done;
            std::unique_lock<std::mutex> lock(*mMutex);
            std::deque<T>::push_back(pItem);
            //logs::debug << LOC() << "Notifying " << &*mIsEmpty << logs::done;
            mIsEmpty->notify_one();
        }

        T pop()
        {
            std::unique_lock<std::mutex> lock(*mMutex);
            while(std::deque<T>::empty())
            {
                //logs::debug << LOC() << "Waiting on " << &*mIsEmpty << logs::done;
                mIsEmpty->wait(lock);
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

        auto size() const
        {
            return std::deque<T>::size();
        }

        T front() const
        {
            return std::deque<T>::operator[](0);
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

        template <class U>
        friend bool operator==(const queue<U>& lhs, const queue<U> rhs);
    
    private:
        std::shared_ptr<std::mutex> mMutex;
        std::shared_ptr<std::condition_variable> mIsEmpty;
};

template <class T>
bool operator==(const queue<T>& lhs, const queue<T> rhs)
{
    return ((std::deque<T>) lhs) == ((std::deque<T>) rhs);
}
