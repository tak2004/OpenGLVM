#ifndef SHAREDATTRIBUTES_TRANSFORM_HPP
#define SHAREDATTRIBUTES_TRANSFORM_HPP
#if _MSC_VER > 1000
#pragma once
#endif

#include <RadonFramework/Math/Geometry/Quaternion.hpp>

namespace Technology { namespace SharedAttributes {

struct Transform
{
    float pos[4];
    RF_Geo::Quaternion<RF_Type::Float32> quaternion;
    float scale[4];
};

} }

#ifndef SHORTHAND_NAMESPACE_ATTRIBUTE
#define SHORTHAND_NAMESPACE_ATTRIBUTE
namespace Attribute = Technology::SharedAttributes;
#endif

#endif // SHAREDATTRIBUTES_TRANSFORM_HPP