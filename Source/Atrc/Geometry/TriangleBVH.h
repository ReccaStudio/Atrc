#pragma once

#include <Atrc/Core/Core.h>

AGZ_NS_BEG(Atrc)

// TriangleBVH�����д��������ݣ������ݴ洢�ֳ���������instancing
class TriangleBVHCore
{
public:

    struct Vertex
    {
        Vec3 pos;
        Vec3 nor;
        Vec2 uv;
    };

    struct Leaf;
    struct Internal;

    using Node = AGZ::TypeOpr::Variant<Leaf, Internal>;

    struct Leaf
    {
        uint32_t start, end;
    };

    struct Internal
    {
        AABB bound;
        uint32_t rightChild;
    };

    struct InternalTriangle
    {
        Vec3 A, B_A, C_A;
        Vec3 nor;
        Vec2 tA, tB_tA, tC_tA;
        Real surfaceArea = 0.0;
    };

    TriangleBVHCore(const Vertex *vertices, uint32_t triangleCount);

    explicit TriangleBVHCore(const AGZ::Model::GeometryMesh &mesh);

    bool HasIntersection(const Ray &r) const;

    bool FindIntersection(const Ray &r, SurfacePoint *sp) const;

    AABB LocalBound() const;

    Real SurfaceArea() const;

    GeometrySampleResult Sample() const;

    const std::vector<InternalTriangle> &GetAllTriangles() const;

private:

    void InitBVH(const Vertex *vertices, uint32_t triangleCount);

    // triangles_�������ǰ׺�ͣ��������ֲ����Խ���ȫ���������ϵ�������Ȳ���
    std::vector<Real> areaPrefixSum_;

    std::vector<InternalTriangle> triangles_;
    std::vector<Node> nodes_;
};

class TriangleBVH : public Geometry
{
    const TriangleBVHCore *core_;
    AABB worldBound_;
    Real surfaceArea_;

public:

    TriangleBVH(const Transform &local2World, const TriangleBVHCore *bvhCore);

    bool HasIntersection(const Ray &r) const override;

    bool FindIntersection(const Ray &r, SurfacePoint *sp) const override;

    Real SurfaceArea() const override;

    AABB LocalBound() const override;

    AABB WorldBound() const override;

    GeometrySampleResult Sample() const override;

    Real SamplePDF(const Vec3 &pos) const override;

    GeometrySampleResult Sample(const Vec3 &dst) const override;

    Real SamplePDF(const Vec3 &pos, const Vec3 &dst) const override;
};

AGZ_NS_END(Atrc)