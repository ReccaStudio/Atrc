#pragma once

#include <Atrc/Lib/Material/Utility/BxDF.h>

namespace Atrc
{

class BxDF_Black : public BxDF
{
public:

    BxDF_Black() noexcept;

    Spectrum GetAlbedo() const noexcept override;

    Spectrum Eval(const CoordSystem &geoInShd, const Vec3 &wi, const Vec3 &wo) const noexcept override;

    Option<SampleWiResult> SampleWi(const CoordSystem &geoInShd, const Vec3 &wo, const Vec2 &sample) const noexcept override;

    Real SampleWiPDF(const CoordSystem &geoInShd, const Vec3 &wi, const Vec3 &wo) const noexcept override;
};

} // namespace Atrc
