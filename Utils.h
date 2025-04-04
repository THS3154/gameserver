#pragma once
#include <random>
#include <chrono>
class Utils
{
public:
    static float GetRandomFloat(float range) {
        static std::random_device rd;  // 난수 생성기
        static std::mt19937 gen(rd()); // Mersenne Twister 엔진
        std::uniform_real_distribution<float> dist(-range, range);

        return dist(gen);
    }

    static int GetRandomInt(int range) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(-range, range);

        return dist(gen);
    }


    static uint64 Millis() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }
};

struct Vector3
{
    float X, Y, Z;

    Vector3() : X(0), Y(0), Z(0) {}
    Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    Vector3 operator+(const Vector3& Other) const
    {
        return Vector3(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
    Vector3 operator-(const Vector3& Other) const
    {
        return Vector3(X - Other.X, Y - Other.Y, Z - Other.Z);
    }

    Vector3 operator*(float Scalar) const
    {
        return Vector3(X * Scalar, Y * Scalar, Z * Scalar);
    }

    Vector3 GetSafeNormal() const
    {
        float Length = sqrt(X * X + Y * Y + Z * Z);
        return (Length > 0.0f) ? Vector3(X / Length, Y / Length, Z / Length) : Vector3();
    }

    static float Distance(const Vector3& A, const Vector3& B)
    {
        return sqrt(pow(B.X - A.X, 2) + pow(B.Y - A.Y, 2) + pow(B.Z - A.Z, 2));
    }
};
