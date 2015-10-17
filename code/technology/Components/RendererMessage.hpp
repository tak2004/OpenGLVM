#ifndef COMPONENTS_RENDERERMESSAGE_HPP
#define COMPONENTS_RENDERERMESSAGE_HPP

#include <RadonFramework/Radon.hpp>

namespace Technology { namespace Components {

class RendererMessage
{
public:
    enum class Type
    {
        OpenGLCommand,
        Initialize
    } What;

    RendererMessage() {}
    RendererMessage(const RendererMessage& Copy) {}

    RF_Mem::AutoPointerArray<RF_Type::UInt8> GLMachineCommandBuffer;
    RF_Type::UInt64 Hash;
};

} }

#ifndef SHORTHAND_NAMESPACE_COMP
#define SHORTHAND_NAMESPACE_COMP
namespace Comp = Technology::Components;
#endif

#endif // COMPONENTS_RENDERERMESSAGE_HPP