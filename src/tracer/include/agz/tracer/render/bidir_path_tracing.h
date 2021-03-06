#pragma once

#include <agz/tracer/core/light.h>
#include <agz/tracer/render/common.h>

AGZ_TRACER_RENDER_BEGIN

namespace bdpt
{

enum class VertexType
{
    Camera,
    AreaLight,
    EnvLight,
    Surface,
    Medium
};

struct Vertex
{
    VertexType type = VertexType::Camera;;

    Spectrum accu_coef;

    real pdf_fwd = 0;
    real pdf_bwd = 0;

    bool is_delta = false;

    Vertex() { }


    struct Camera
    {
        Vec3 pos;
        Vec3 nor;
    };

    struct Surface
    {
        Vec3 pos;
        Vec3 nor;
        Vec2 uv;

        Vec3 wr;

        const Medium *med_out = nullptr;
        const Medium *med_in = nullptr;

        const BSDF   *bsdf   = nullptr;
        const Entity *entity = nullptr;

        const Medium *medium(const Vec3 &wr) const noexcept
        {
            return dot(wr, nor) > 0 ? med_out : med_in;
        }
    };

    struct AreaLight
    {
        Vec3 pos;
        Vec3 nor;
        Vec2 uv;

        const tracer::AreaLight *light = nullptr;
    };

    struct EnvLight
    {
        Vec3 light_to_out;
    };

    struct Medium
    {
        Vec3 pos;
        Vec3 wr;

        const tracer::Medium *med = nullptr;

        const BSDF *phase = nullptr;
    };
    
    union
    {
        Camera    camera;
        Surface   surface;
        AreaLight area_light;
        EnvLight  env_light;
        Medium    medium;
    };

    bool is_scattering_type() const noexcept
    {
        return type == VertexType::Surface || type == VertexType::Medium;
    }
};

struct Subpath
{
    int vertex_count = 0;
    Vertex *vertices = nullptr;
};

struct CameraSubpath : Subpath
{
    // gbuffer channels

    FSpectrum g_albedo;
    FVec3 g_normal;
    real g_denoise = 1;
};

struct LightSubpath : Subpath
{

};

CameraSubpath build_camera_subpath(
    int max_vertex_count, const Ray &ray,
    const Scene &scene, Sampler &sampler,
    Arena &arena, Vertex *vertex_space);

LightSubpath build_light_subpath(
    int max_vertex_count,
    const SceneSampleLightResult &select_light,
    const Scene &scene, Sampler &sampler,
    Arena &arena, Vertex *vertex_space);

FSpectrum contrib_s2_t0(
    const Scene &scene,
    const Vertex *camera_subpath);

FSpectrum unweighted_contrib_sx_t0(
    const Scene &scene,
    const Vertex *camera_subpath, int s);

FSpectrum unweighted_contrib_sx_t1(
    const Scene &scene,
    const Vertex *camera_subpath, int s,
    const Vertex *light_subpath,
    Sampler &sampler);

FSpectrum unweighted_contrib_s1_tx(
    const Scene &scene,
    const Vertex *camera_subpath,
    const Vertex *light_subpath, int t,
    Sampler &sampler,
    const Rect2 &sample_pixel_bound,
    const Vec2 &full_res,
    Vec2 &pixel_coord);

FSpectrum unweighted_contrib_sx_tx(
    const Scene &scene,
    const Vertex *camera_subpath, int s,
    const Vertex *light_subpath, int t,
    Sampler &sampler);

real mis_weight_sx_t0(
    const Scene &scene,
    Vertex *camera_subpath, int s);

real mis_weight_sx_t1(
    const Scene &scene,
    Vertex *camera_subpath, int s,
    Vertex *light_subpath);

real mis_weight_s1_tx(
    const Scene &scene,
    Vertex *camera_subpath,
    Vertex *light_subpath, int t);

real mis_weight_sx_tx(
    Vertex *camera_subpath, int s,
    Vertex *light_subpath, int t);

FSpectrum weighted_contrib_sx_t0(
    const Scene &scene,
    Vertex *camera_subpath, int s);

FSpectrum weighted_contrib_sx_t1(
    const Scene &scene,
    Vertex *camera_subpath, int s,
    Vertex *light_subpath,
    Sampler &sampler);

FSpectrum weighted_contrib_s1_tx(
    const Scene &scene,
    Vertex *camera_subpath,
    Vertex *light_subpath, int t,
    Sampler &sampler,
    const Rect2 &sample_pixel_bound,
    const Vec2 &full_res,
    Vec2 &pixel_coord);

FSpectrum weighted_contrib_sx_tx(
    const Scene &scene,
    Vertex *camera_subpath, int s,
    Vertex *light_subpath, int t,
    Sampler &sampler);

struct EvalBDPTPathParams
{
    const Scene &scene;
    const Rect2 sample_pixel_bound;
    const Vec2 full_res;
    Sampler &sampler;
};

template<bool UseMIS, typename ParticleFunc>
FSpectrum eval_bdpt_path(
    const EvalBDPTPathParams &params,
    Vertex *camera_subpath, int camera_vertex_count,
    Vertex *light_subpath, int light_vertex_count,
    const SceneSampleLightResult &select_light,
    ParticleFunc &&particle_func)
{
    FSpectrum ret;

    for(int s = 1; s <= camera_vertex_count; ++s)
    {
        for(int t = 0; t <= light_vertex_count; ++t)
        {
            const int s_t = s + t;

            if(s_t < 2)
                continue;

            if(s == 2 && t == 0)
            {
                ret += contrib_s2_t0(params.scene, camera_subpath);
                continue;
            }

            if(s_t < 3)
                continue;

            if(s == 1)
            {
                assert(t >= 2);

                Vec2 particle_pixel_coord;

                FSpectrum particle_rad;

                if constexpr(UseMIS)
                {
                    particle_rad = weighted_contrib_s1_tx(
                        params.scene, camera_subpath,
                        light_subpath, t, params.sampler,
                        params.sample_pixel_bound,
                        params.full_res,
                        particle_pixel_coord);
                }
                else
                {
                    particle_rad = unweighted_contrib_s1_tx(
                        params.scene, camera_subpath,
                        light_subpath, t, params.sampler,
                        params.sample_pixel_bound,
                        params.full_res,
                        particle_pixel_coord) / real(s_t);
                }

                if(particle_rad.is_finite() && !particle_rad.is_black())
                    particle_func(particle_pixel_coord, particle_rad);

                continue;
            }

            if(t == 1)
            {
                assert(s >= 2);

                if constexpr(UseMIS)
                {
                    ret += weighted_contrib_sx_t1(
                        params.scene, camera_subpath, s, light_subpath,
                        params.sampler);
                }
                else
                {
                    ret += unweighted_contrib_sx_t1(
                        params.scene, camera_subpath, s, light_subpath,
                        params.sampler) / real(s_t);
                }

                continue;
            }

            if(t == 0)
            {
                assert(s >= 3);

                if constexpr(UseMIS)
                {
                    ret += weighted_contrib_sx_t0(
                        params.scene, camera_subpath, s);
                }
                else
                {
                    ret += unweighted_contrib_sx_t0(
                        params.scene, camera_subpath, s) / real(s_t);
                }

                continue;
            }

            assert(s >= 2 && t >= 2);

            if constexpr(UseMIS)
            {
                ret += weighted_contrib_sx_tx(
                    params.scene,
                    camera_subpath, s,
                    light_subpath, t,
                    params.sampler);
            }
            else
            {
                ret += unweighted_contrib_sx_tx(
                    params.scene,
                    camera_subpath, s,
                    light_subpath, t,
                    params.sampler) / real(s_t);
            }
        }
    }

    return ret;
}

} // namespace bdpt

AGZ_TRACER_RENDER_END
