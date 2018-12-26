#include <Atrc/Mgr/Context.h>

namespace Atrc::Mgr
{

void RegisterBuiltinName2GeometryCreators(Context &context);

/*
    type = WavefrontOBJ
    filename = Filename
*/
class WavefrontOBJName2GeometryCreator : public Creator<Name2Geometry>
{
    mutable std::unordered_map<Str8, Name2Geometry*> path2rt_;

public:

    Str8 GetTypeName() const override { return "WavefrontOBJ"; }

    Name2Geometry *Create(const ConfigGroup &group, Context &context, Arena &arena) const override;
};

} // namespace Atrc::Mgr