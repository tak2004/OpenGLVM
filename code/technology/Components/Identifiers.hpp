#ifndef COMPONENTS_IDENTIFIERS_HPP
#define COMPONENTS_IDENTIFIERS_HPP
#if _MSC_VER > 1000
#pragma once
#endif

namespace Technology { namespace Components {

namespace Identifiers {
    enum Type : RF_Type::UInt32
    {
        RenderBehaviour = 0,
        CameraBehaviour,

        MAX
    };
}

} }

#ifndef SHORTHAND_NAMESPACE_COMP
#define SHORTHAND_NAMESPACE_COMP
namespace Comp = Technology::Components;
#endif

#endif // COMPONENTS_IDENTIFIERS_HPP