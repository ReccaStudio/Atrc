#pragma once

#include <Atrc/Lib/Core/CoordSystem.h>
#include <Atrc/Lib/Material/Utility/MicrofacetDistribution.h>

namespace Atrc
{
    
class BlinnPhong : public MicrofacetDistribution
{
    Real e_;

public:

    explicit BlinnPhong(Real e) noexcept;

    Real D(const Vec3 &H) const noexcept override;

    Real G(const Vec3 &H, const Vec3 &wi, const Vec3 &wo) const noexcept override;

    Option<SampleWiResult> SampleWi(const CoordSystem &geoInShd, const Vec3 &wo, const Vec2 &sample) const noexcept override;

    Real SampleWiPDF(const CoordSystem &geoInShd, const Vec3 &wi, const Vec3 &wo) const noexcept override;
};

} // namespace Atrc
