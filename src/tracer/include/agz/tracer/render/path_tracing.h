#pragma once

#include <agz/tracer/render/common.h>

AGZ_TRACER_RENDER_BEGIN

struct TraceParams
{
    int min_depth  = 5;
    int max_depth  = 10;
    real cont_prob = real(0.9);

    int direct_illum_sample_count = 1;

    // additional depth for specular scattering
    int specular_depth = 20;
};

struct AOParams
{
    FSpectrum background_color;
    FSpectrum low_color;
    FSpectrum high_color;

    int ao_sample_count = 4;
    real max_occlusion_distance = 1;
};

struct AlbedoAOParams
{
    int ao_sample_count = 4;
    real max_occlusion_distance = 1;
};

Pixel trace_std(
    const TraceParams &params,
    const Scene &scene, const Ray &ray,
    Sampler &sampler, Arena &arena);

Pixel trace_nomis(
    const TraceParams &params,
    const Scene &scene, const Ray &ray,
    Sampler &sampler, Arena &arena);

Pixel trace_ao(
    const AOParams &params,
    const Scene &scene, const Ray &ray,
    Sampler &sampler);

Pixel trace_albedo_ao(
    const AlbedoAOParams &params,
    const Scene &scene, const Ray &ray,
    Sampler &sampler, Arena &arena);

AGZ_TRACER_RENDER_END
