//Yuntian Liu
//C00151315
#ifndef STACK_H
#define STACK_H
#include <iostream>
#include <list>

using namespace std;

class CStack
{
public:
    void Push(void *pData)
    {
        m_dataList.push_back(pData);
    }
    void* Pop()
    {
        void *ptr = NULL;
        list<void *>::iterator it = m_dataList.end();

        it--;
        ptr =  *it;
        m_dataList.pop_back();

        return ptr;
    }

    void Clear()
    {
        m_dataList.clear();
    }

    bool IsEmpty()
    {
        return m_dataList.empty();
    }

    size_t GetSize()
    {
        return m_dataList.size();
    }
protected:
    list<void *> m_dataList;
};

#endif
