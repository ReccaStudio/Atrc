#pragma once

#include <Atrc/Common.h>
#include <Atrc/Entity/Entity.h>
#include <Atrc/Material/Material.h>

AGZ_NS_BEG(Atrc)

template<typename GeoTpl,
         std::enable_if_t<std::is_base_of_v<Entity, GeoTpl>, int> = 0>
class MatGeoEntity
    : public GeoTpl
{
    RC<Material> mat_;

    template<typename U>
    Vec2r GetMaterialParameter(const Intersection &inct, decltype(&U::GetMaterialParameter)) const
    {
        return this->GeoTpl::GetMaterialParameter(inct);
    }

    template<typename U>
    Vec2r GetMaterialParameter(const Intersection &inct, ...) const
    {
        return inct.uv;
    }

public:

    template<typename...Args>
    explicit MatGeoEntity(RC<Material> mat, Args&&...args)
        : GeoTpl(std::forward<Args>(args)...), mat_(std::move(mat))
    {
        
    }

    RC<BxDF> GetBxDF(const Intersection &inct) const override
    {
        return mat_->GetBxDF(inct, GetMaterialParameter<GeoTpl>(inct, 0));
    }
};

AGZ_NS_END(Atrc)