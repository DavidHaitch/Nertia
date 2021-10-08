#ifndef VEC3_H
#define VEC3_H
class Vec3
{
public:
    float x;
    float y;
    float z;
    Vec3()
    {
        x = y =z = 0;
    }

    Vec3(float x_, float y_, float z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }

    float Dot(Vec3 vec)
    {
        float product = 0;
        product += x * vec.x;
        product += y * vec.y;
        product += z * vec.z;
        return product;
    }

    Vec3 Cross(Vec3 vec)
    {
        Vec3 cross(0, 0, 0);
        cross.x = y * vec.z - z * vec.y;
        cross.y = -(x * vec.z - z * vec.x);
        cross.z = x * vec.y - y * vec.x;
        return cross;
    }

    friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
    {
        return Vec3(rhs.x + lhs.x, rhs.y * lhs.y, rhs.z * lhs.z);
    }

    friend Vec3 operator*(float lhs, const Vec3& rhs)
    {
        return Vec3(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
    }
};
#endif