#include <agz/tracer/core/bsdf.h>
#include <agz/tracer/core/material.h>

AGZ_TRACER_BEGIN

namespace
{
    class InvisibleSurfaceBSDF : public BSDF
    {
        Vec3 geometry_normal_;

    public:

        explicit InvisibleSurfaceBSDF(const Vec3 &geometry_normal) noexcept
            : geometry_normal_(geometry_normal)
        {
            
        }

        Spectrum eval(
            const Vec3 &wi, const Vec3 &wo, TransMode mode) const noexcept override
        {
            return {};
        }

        BSDFSampleResult sample(
            const Vec3 &wo, TransMode mode,
            const Sample3 &sam) const noexcept override
        {
            const real cosv = std::abs(cos(geometry_normal_, wo));

            BSDFSampleResult ret;
            ret.dir      = -wo;
            ret.f        = Spectrum(1) / (cosv < EPS ? 1 : cosv);
            ret.pdf      = 1;
            ret.is_delta = true;
            return ret;
        }

        real pdf(const Vec3 &wi, const Vec3 &wo) const noexcept override
        {
            return 0;
        }

        Spectrum albedo() const noexcept override
        {
            return Spectrum(1);
        }

        bool is_delta() const noexcept override
        {
            return true;
        }

        bool has_diffuse_component() const noexcept override
        {
            return false;
        }
    };
}

class InvisibleSurfaceMaterial : public Material
{
public:

    ShadingPoint shade(const SurfacePoint &inct, Arena &arena) const override
    {
        ShadingPoint shd;
        shd.bsdf = arena.create<InvisibleSurfaceBSDF>(
                                inct.geometry_coord.z);
        shd.shading_normal = inct.user_coord.z;
        return shd;
    }
};

RC<Material> create_invisible_surface()
{
    return newRC<InvisibleSurfaceMaterial>();
}

AGZ_TRACER_END