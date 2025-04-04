#include "pch.h"
#include "Object.h"
#include "Utils.h"
Object::Object()
{
	objectInfo = new Protocol::ObjectInfo();
	posInfo = objectInfo->mutable_posinfo();
	movePosInfo = new Protocol::PosInfo();
}

Object::~Object()
{
	delete objectInfo;
}

float Object::Distance3D(const float X, const float Y, const float Z)
{
	return std::sqrt(std::pow(posInfo->x() - X, 2) + std::pow(posInfo->y() - Y, 2) + std::pow(posInfo->z() - Z, 2));
}

float Object::Distance2D(const float X, const float Y)
{
	return std::sqrt(std::pow(posInfo->x() - X, 2) + std::pow(posInfo->y() - Y, 2));
}

float Object::Distance2D(const Vector3 src, const Vector3 dest)
{
	return std::sqrt(std::pow(src.X - dest.X, 2) + std::pow(src.Y - dest.Y, 2));
}
