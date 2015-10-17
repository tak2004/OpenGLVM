#ifndef SHAREDATTRIBUTES_IDENTIFIERS_HPP
#define SHAREDATTRIBUTES_IDENTIFIERS_HPP
#if _MSC_VER > 1000
#pragma once
#endif

namespace Technology { namespace SharedAttributes {

namespace Identifiers
{
    enum Type
    {
        Transform = 0,

        MAX
    };
}

} }

#ifndef SHORTHAND_NAMESPACE_ATTRIBUTE
#define SHORTHAND_NAMESPACE_ATTRIBUTE
namespace Attribute = Technology::SharedAttributes;
#endif

#endif // SHAREDATTRIBUTES_IDENTIFIERS_HPP