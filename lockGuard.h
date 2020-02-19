#pragma once

namespace lckg{
    template<class mutex_type>
    class lock_guard  {
        mutex_type& m_mutex;
    public:
        lock_guard (mutex_type &m_lock) : m_mutex(m_lock) { m_mutex.lock(); }
        ~lock_guard () { m_mutex.unlock(); }
    };
}
