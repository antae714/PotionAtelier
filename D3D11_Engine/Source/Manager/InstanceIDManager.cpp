#include "InstanceIDManager.h"
#include <ranges>
#include <algorithm>

InstanceIDManager& instanceIDManager = InstanceIDManager::GetInstance();

InstanceIDManager::InstanceIDManager()
{
  
}

InstanceIDManager::~InstanceIDManager()
{

}

unsigned int InstanceIDManager::getUniqueID()
{
    std::lock_guard lock(mutex);
    if (!availableIDs.empty())
    {
        unsigned int id = availableIDs.front();
        availableIDs.pop_front();
        activeIDs.insert(id);
        return id;
    }

    // availableIDs�� ������� ���ο� ID�� ����
    unsigned int newID = nextID++;
    activeIDs.insert(newID);
    return newID;
}

void InstanceIDManager::returnID(unsigned int id)
{
    std::lock_guard lock(mutex);
    if (activeIDs.find(id) != activeIDs.end()) 
    {
        activeIDs.erase(id);
        availableIDs.push_back(id);
    }
    else
    {
        __debugbreak(); //�ν��Ͻ� ���̵� ����.
    }
}

void InstanceIDManager::SortReturnID()
{
    std::lock_guard lock(mutex);
    std::ranges::sort(availableIDs, [](unsigned int a, unsigned int b) {return a < b;});
}
