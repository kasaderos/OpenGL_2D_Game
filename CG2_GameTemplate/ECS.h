#pragma once

/*
    Actually it's something simillar to ECS,
    but far from pure realization.
*/

#include <typeindex>

#include "stl.h"

#define ECS_ENTITY_DEFAULTS(entityType) \
    static IEntity *_Create() { return new entityType(); } \
    entityType() {}
    // template<typename ...Args> entityType(const Args &...args) : Entity<Args...>(args...) {}
    // template<typename ...Args> static IEntity *_Create(const Args &...args) { return new entityType(args...); }

#define ECS_SYSTEM_DEFAULTS(sysName) \
    static IEntity *_Create() { return new sysName(); } \
    sysName() {}

namespace ecs
{

typedef std::type_index EntityTypeId;
typedef std::type_index ComponentTypeId;
template <typename T>
EntityTypeId GetEntityTid() { return EntityTypeId(typeid(T)); }
template <typename T>
ComponentTypeId GetComponentTid() { return ComponentTypeId(typeid(T)); }

struct Component
{
    // Not virtual. It's just for not to write
    // implementation in each component type.
    // (you may not need to load some components from files,
    // also you need not to load empty components).
    void FromString(const char *str) {}
};

class EntityDesc
{
public:
    EntityDesc(EntityTypeId tid, unsigned id) : tid(tid), id(id) {}
    EntityTypeId tid;
    unsigned id; // Ids may be same for different types.
};

class IEntity
{
public:
    //IEntity() : desc(typeid(typeid(*this)), 0) {}
    IEntity() : desc(typeid(void), 0) {}
    virtual void OnUpdate() = 0;
    virtual void OnCreated() {}
    virtual void OnDestroy() {}
    const EntityDesc &GetDesc() { return desc; }

    template <typename T>
    T *GetComp() { return (T *)GetCompRaw(GetComponentTid<T>()); }
    virtual void *GetCompRaw(ComponentTypeId tid) { return nullptr; }
    virtual bool SetComp(const char *compName, const char *value) { return false; }

private:
    EntityDesc desc;
    friend class EntityManager;
};

typedef IEntity System;

template<typename T, typename ...Other>
class Entity : public Entity<Other...>
{
public:

    Entity() {}
    Entity(Other ...other) : Entity<Other...>(other...) {}
    Entity(T comp, Other ...other) : myComp(comp), Entity<Other...>(other...) {}

    void *GetCompRaw(ComponentTypeId tid) override
    {
        if (tid == GetComponentTid<T>())
            return &myComp;
        return Entity<Other...>::GetCompRaw(tid);
    }

    bool SetComp(const char *compName, const char *value) override
    {
        //std::cout << GetComponentTid<T>().name() << std::endl;

        // Hack: all component type names are 'struct <TypeName>'
        // compName is <TypeName>. So all we need is to compare
        // compName and type name of T starting from 7-th position.
        if (!strcmp(compName, GetComponentTid<T>().name() + 7))   
        {
            myComp.FromString(value);
            return true;
        }
        else
            return Entity<Other...>::SetComp(compName, value);
    }

private:

    T myComp;
};

template<typename T>
class Entity<T> : public IEntity
{
public:

    Entity() {}
    Entity(T comp) : myComp(comp) {}

    void *GetCompRaw(ComponentTypeId tid) override
    {
        if (tid == GetComponentTid<T>())
            return &myComp;
        return nullptr;
    }

    bool SetComp(const char *compName, const char *value) override
    {
        //std::cout << GetComponentTid<T>().name() << std::endl;
        if (!strcmp(compName, GetComponentTid<T>().name() + 7))
        {
            myComp.FromString(value);
            return true;
        }
        return false;
    }

    void OnUpdate() override {}

private:

    T myComp;
};

// Applies the specified function to each entity
// which has all of the specified components.
template <typename ...Args>
void Apply(stl::function<void(IEntity *)> &&func)
{
    auto poolsList = ecs::gEntityManager.GetEntitiesByComp({ GetComponentTid<Args>()... });
    for (auto &pool : poolsList) {
        for (auto &entity : *pool) {
            if (entity) {
                func(entity.get());
            }
        }
    }
}

class EntityManager
{
public:

    static void AddRegisteredSystems();

    template<typename T, typename ...Args>
    EntityDesc CreateEntity(Args &&...args)
    {
        EntityTypeId tid = GetEntityTid<T>();
        auto pool = entityPools.find(tid);
        if (pool == entityPools.end())
        {
            entityPools.emplace(tid, EntityPool());
            pool = entityPools.find(tid);
            if (GetForwardTypePriority(tid) < 0 && GetBackwardTypePriority(tid) < 0)
                unorderedPoolIds.push_back(tid);
        }
        IEntity *ent = new T(args...);
        if (!ent)
            abort();
        ent->desc.id = pool->second.AddEntity(ent);
        ent->desc.tid = tid;
        ent->OnCreated();
        return ent->desc;
    }

    // If the template isn't registered, 'success' will be false
    // and nothing will happen.
    EntityDesc CreateEntityByName(const char *templateName, bool &success);

    void DestroyEntity(EntityDesc desc);

    void DestroyAllEntities();

    // There could be null pointers. Just don't do with them anything.
    stl::list<stl::vector<stl::unique_ptr<IEntity>> *> GetEntitiesByComp(ComponentTypeId ctid);
    
    stl::list<stl::vector<stl::unique_ptr<IEntity>> *> GetEntitiesByComp(std::initializer_list<ComponentTypeId> ctids);
    
    // Returns nullptr if not found.
    // There could be null pointers. Just don't do with them anything.
    stl::vector<stl::unique_ptr<IEntity>> *GetEntitiesByType(EntityTypeId etid);

    // Returns nullptr if not found.
    // Do not store or delete this pointer.
    IEntity *GetEntityByDesc(EntityDesc desc);

    void UpdateEntities();

private:

    struct EntityPool
    {
        stl::vector<unsigned> freeIds;
        stl::vector<stl::unique_ptr<IEntity>> entities;
        int AddEntity(IEntity *ent);
        void RemoveEntity(unsigned id);
        void RemoveAllEntities();
        void OnUpdate();
    };

    stl::map<EntityTypeId, EntityPool> entityPools;
    stl::vector<EntityTypeId> unorderedPoolIds;
    
    static EntityTypeId forwardUpdateOrder[];
    static EntityTypeId backwardUpdateOrder[];
    static const unsigned forwardUpdateOrderSize;
    static const unsigned backwardUpdateOrderSize;

    static int GetForwardTypePriority(EntityTypeId tid);
    static int GetBackwardTypePriority(EntityTypeId tid);
};

extern EntityManager gEntityManager;

}