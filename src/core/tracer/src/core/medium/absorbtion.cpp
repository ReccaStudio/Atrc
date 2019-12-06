﻿#include <agz/tracer/core/medium.h>
#include <agz/utility/misc.h>

AGZ_TRACER_BEGIN

class AbsorbtionMedium : public Medium
{
    Spectrum sigma_a_; // 吸收系数

public:

    void initialize(const Spectrum &sigma_a)
    {
        sigma_a_ = sigma_a;
    }

    Spectrum tr(const Vec3 &a, const Vec3 &b, Sampler &sampler) const noexcept override
    {
        Spectrum exp = -sigma_a_ * (a - b).length();
        return {
            std::exp(exp.r),
            std::exp(exp.g),
            std::exp(exp.b)
        };
    }

    SampleOutScatteringResult sample_scattering(const Vec3 &a, const Vec3 &b, Sampler &sampler, Arena &arena) const noexcept override
    {
        Spectrum tr_value = tr(a, b, sampler);
        return { { }, tr_value, nullptr };
    }
};

std::shared_ptr<Medium> create_absorbtion_medium(
    const Spectrum &sigma_a)
{
    auto ret = std::make_shared<AbsorbtionMedium>();
    ret->initialize(sigma_a);
    return ret;
}

AGZ_TRACER_END