#include <agz/tracer/core/aggregate.h>
#include <agz/tracer/core/entity.h>

AGZ_TRACER_BEGIN

class NativeAggregate : public Aggregate
{
    std::vector<std::shared_ptr<const Entity>> entities_;
    std::vector<const Entity*> raw_entities_;
    AABB world_bound_;

public:

    void build(const std::vector<std::shared_ptr<const Entity>> &entities) override
    {
        raw_entities_.clear();
        entities_ = entities;
        for(size_t i = 0; i < entities.size(); ++i)
        {
            raw_entities_.push_back(entities[i].get());
            world_bound_ |= entities[i]->world_bound();
        }
    }

    bool has_intersection(const Ray &r) const noexcept override
    {
        for(auto ent : raw_entities_)
        {
            if(ent->has_intersection(r))
                return true;
        }
        return false;
    }

    bool closest_intersection(const Ray &r, EntityIntersection *inct) const noexcept override
    {
        Ray ray = r;
        bool ret = false;
        for(auto ent : raw_entities_)
        {
            if(ent->closest_intersection(ray, inct))
            {
                ray.t_max = inct->t;
                ret = true;
            }
        }
        return ret;
    }

    AABB world_bound() const noexcept override
    {
        return world_bound_;
    }
};

std::shared_ptr<Aggregate> create_native_aggregate()
{
    auto ret = std::make_shared<NativeAggregate>();
    return ret;
}

AGZ_TRACER_END