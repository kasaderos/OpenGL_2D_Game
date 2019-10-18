#include "ECS.h"

namespace ecs
{

stl::map<stl::string, stl::pair<EntityTypeId, IEntity *(*)()>> registeredEntityTypes;

struct RegisterEntityTypeInConstructor
{
    RegisterEntityTypeInConstructor(const char *name, const stl::pair<EntityTypeId, IEntity *(*)()> &info)
    {
        registeredEntityTypes.emplace(name, info);
    }
};

EntityManager gEntityManager;

stl::set<stl::string> registeredToAdd;

template<typename T>
struct RegisterSystemInConstructor
{
    RegisterSystemInConstructor(const char *name)
    {
        registeredToAdd.emplace(name);
    }
};

void EntityManager::AddRegisteredSystems()
{
    for (auto &sys : registeredToAdd)
    {
        bool success;
        gEntityManager.CreateEntityByName(sys.c_str(), success);
    }
}

EntityDesc EntityManager::CreateEntityByName(const char *templateName,  bool &success)
{
    success = true;
    auto templ = registeredEntityTypes.find(templateName);
    if (templ == registeredEntityTypes.end())
    {
        success = false;
        printf("Entity creation failed: unknown template %s\n", templateName);
        return EntityDesc(GetEntityTid<void>(), 0);
    }
    EntityTypeId tid = templ->second.first;
    auto pool = entityPools.find(tid);
    if (pool == entityPools.end())
    {
        entityPools.emplace(tid, EntityPool());
        pool = entityPools.find(tid);
        if (GetForwardTypePriority(tid) < 0 && GetBackwardTypePriority(tid) < 0)
            unorderedPoolIds.push_back(tid);
    }
    IEntity *ent = templ->second.second();
    if (!ent)
        abort();
    ent->desc.id = pool->second.AddEntity(ent);
    ent->desc.tid = tid;
    ent->OnCreated();
    return ent->desc;
}

void EntityManager::DestroyEntity(EntityDesc desc)
{
    auto pool = entityPools.find(desc.tid);
    if (pool != entityPools.end())
        pool->second.RemoveEntity(desc.id);
}

void EntityManager::DestroyAllEntities()
{
    for (auto &pool : entityPools)
        pool.second.RemoveAllEntities();
}

stl::list<stl::vector<stl::unique_ptr<IEntity>> *> EntityManager::GetEntitiesByComp(ComponentTypeId ctid)
{
    stl::list<stl::vector<stl::unique_ptr<IEntity>> *> out;
    for (auto &pool : entityPools)
    {
        for (auto &ptr : pool.second.entities)
        {
            if (ptr && ptr->GetCompRaw(ctid))
            {
                // If one entity in the pool have the specified
                // component, then the overs have too, since they
                // are of the same type.
                out.push_front(&pool.second.entities);
                break;
            }
        }
    }
    return out;
}

stl::list<stl::vector<stl::unique_ptr<IEntity>> *> EntityManager::GetEntitiesByComp(std::initializer_list<ComponentTypeId> ctids)
{
    stl::list<stl::vector<stl::unique_ptr<IEntity>> *> out;
    for (auto &pool : entityPools)
    {
        for (auto &ptr : pool.second.entities)
        {
            if (ptr)// && ptr->GetCompRaw(ctid))
            {
                bool all = true;
                for (auto &ctid : ctids)
                    if (!ptr->GetCompRaw(ctid))
                    {
                        all = false;
                        break;
                    }
                if (all)
                    // If one entity in the pool have the specified
                    // component, then the overs have too, since they
                    // are of the same type.
                    out.push_front(&pool.second.entities);
                break;
            }
        }
    }
    return out;
}

stl::vector<stl::unique_ptr<IEntity>> *EntityManager::GetEntitiesByType(EntityTypeId etid)
{
    auto pool = entityPools.find(etid);
    if (pool != entityPools.end())
        return &pool->second.entities;
    return nullptr;
}

IEntity *EntityManager::GetEntityByDesc(EntityDesc desc)
{
    auto pool = entityPools.find(desc.tid);
    if (pool != entityPools.end())
    {
        stl::vector<stl::unique_ptr<IEntity>> &entities = pool->second.entities;
        if (desc.id < entities.size())
            return entities[desc.id].get(); // May be null.
    }
    return nullptr;
}

void ecs::EntityManager::UpdateEntities()
{
    for (unsigned i = 0; i < forwardUpdateOrderSize; ++i)
    {
        auto pool = entityPools.find(forwardUpdateOrder[i]);
        if (pool != entityPools.end())
            pool->second.OnUpdate();
    }
    for (auto poolId : unorderedPoolIds)
        entityPools[poolId].OnUpdate();
    for (unsigned i = 0; i < backwardUpdateOrderSize; ++i)
    {
        auto pool = entityPools.find(backwardUpdateOrder[i]);
        if (pool != entityPools.end())
            pool->second.OnUpdate();
    }
}

int EntityManager::GetForwardTypePriority(EntityTypeId tid)
{
    for (unsigned i = 0; i < forwardUpdateOrderSize; ++i)
        if (tid == forwardUpdateOrder[i])
            return i;
    return -1;
}

int EntityManager::GetBackwardTypePriority(EntityTypeId tid)
{
    for (unsigned i = 0; i < backwardUpdateOrderSize; ++i)
        if (tid == backwardUpdateOrder[i])
            return i;
    return -1;
}

int EntityManager::EntityPool::AddEntity(IEntity *ent)
{
    if (freeIds.size())
    {
        int id = freeIds.back();
        freeIds.pop_back();
        entities[id].reset(ent);
        return id;
    }
    else
    {
        entities.push_back(stl::unique_ptr<IEntity>(ent));
        return int(entities.size() - 1);
    }
}

void EntityManager::EntityPool::RemoveEntity(unsigned id)
{
    if (id >= entities.size())
        return;

    //if (id == entities.size() - 1)
    //{
    //    entities.back()->OnDestroy();
    //    entities.back().reset();
    //    entities.pop_back();
    //}
    //else

    // Never pop_back().
    {
        if (entities[id])
        {
            entities[id]->OnDestroy();
            entities[id].reset();
            freeIds.push_back(id);
        }
    }
}

void EntityManager::EntityPool::RemoveAllEntities()
{
    for (unsigned i = 0; i < entities.size(); ++i)
    {
        RemoveEntity(i);
    }
    freeIds.clear();
    entities.clear();
}

void EntityManager::EntityPool::OnUpdate()
{
    for (auto &elem : entities)
        if (elem)
            elem->OnUpdate();
}

}

#define ECS_REGISTER_ENTITY_TYPE(typeName) \
    static ecs::RegisterEntityTypeInConstructor ecs_tn_##typeName(#typeName, \
    stl::pair<ecs::EntityTypeId, ecs::IEntity *(*)()>(ecs::GetEntityTid<typeName>(), typeName::_Create));
#include "EcsEntities.h"

#define ECS_REGISTER_SYSTEM(sysName) \
    ECS_REGISTER_ENTITY_TYPE(sysName) \
    static ecs::RegisterSystemInConstructor<sysName> ecs_es_##sysName(#sysName);
#include "EcsSystems.h"