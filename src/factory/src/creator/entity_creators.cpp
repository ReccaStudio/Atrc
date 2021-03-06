#include <agz/tracer/core/medium.h>
#include <agz/factory/creator/entity_creators.h>
#include <agz/tracer/create/entity.h>
#include <agz/tracer/create/medium.h>

AGZ_TRACER_FACTORY_BEGIN

namespace entity
{

    MediumInterface create_medium_interface(
        const ConfigGroup &params, CreatingContext &context)
    {
        MediumInterface ret;

        if(auto node = params.find_child_group("med_in"))
            ret.in = context.create<Medium>(node->as_group());
        else
            ret.in = create_void();

        if(auto node = params.find_child_group("med_out"))
            ret.out = context.create<Medium>(node->as_group());
        else
            ret.out = create_void();

        return ret;
    }
    
    class GeometricEntityCreator : public Creator<Entity>
    {
    public:

        std::string name() const override
        {
            return "geometric";
        }

        RC<Entity> create(
            const ConfigGroup &params, CreatingContext &context) const override
        {
            auto geometry = context.create<Geometry>(
                params.child_group("geometry"));

            auto material = context.create<Material>(
                params.child_group("material"));

            const auto medium = create_medium_interface(params, context);

            const auto emit_rad = params.child_spectrum_or(
                "emit_radiance", Spectrum());

            const auto no_denoise = params.child_int_or("no_denoise", 0);

            const real power = params.child_real_or("power", -1);

            return create_geometric(
                std::move(geometry), std::move(material),
                medium, emit_rad, no_denoise, power);
        }
    };

} // namespace entity

void initialize_entity_factory(Factory<Entity> &factory)
{
    factory.add_creator(newBox<entity::GeometricEntityCreator>());
}

AGZ_TRACER_FACTORY_END
