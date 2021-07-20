#pragma once
#include <cstdint>

namespace Crystal {
	template<class To, class From>
	concept ExplicitConvertible = requires (From& f) { static_cast<To>(f); };

    constexpr uint32_t ALL_SUBRESOURCES = 0xffffffff;

    enum class IndexFormat_t {
        unknown = 0,
        uint_16 = 57,
        uint_32 = 42
    };

    enum class CommandListType_t {
        direct,
        bundle,
        compute,
        copy
    };

    enum class IndexFormat_t {
        unknown = 0,
        uint_16 = 57,
        uint_32 = 42
    };

    enum class ResourceState_t {
        common                            = 0,
        vertex_and_constant_buffer        = 0x1,
        index_buffer                      = 0x2,
        render_target                     = 0x4,
        unordered_access                  = 0x8,
        depth_write                       = 0x10,
        depth_read                        = 0x20,
        non_pixel_shader_resource         = 0x40,
        pixel_shader_resource             = 0x80,
        stream_out                        = 0x100,
        indirect_argument                 = 0x200,
        copy_dest                         = 0x400,
        copy_source                       = 0x800,
        resolve_dest                      = 0x1000,
        resolve_source                    = 0x2000,
        raytracing_acceleration_structure = 0x400000,
        shading_rate_source               = 0x1000000,
        present                           = 0,
        predication                       = 0x200,
        generic_read                      = vertex_and_constant_buffer |
                                            index_buffer               |
                                            non_pixel_shader_resource  |
                                            pixel_shader_resource      |
                                            indirect_argument          |
                                            copy_source
    };

    enum class Topology_t {
        undefined     = 0,
        pointlist     = 1,
        linelist      = 2,
        linestrip     = 3,
        trianglelist  = 4,
        trianglestrip = 5,
    };

    enum class ClearFlag_t {
        depth   = 0x1,
        stencil = 0x2
    };

    struct ClearFlag {
        template<ExplicitConvertible<ClearFlag_t> T>
        [[nodiscard]] constexpr T As() const noexcept { return static_cast<T>(clearFlag); }

        ClearFlag_t clearFlag;
    };

    struct PrimitiveTopology {
        template<ExplicitConvertible<Topology_t> T>
        [[nodiscard]] constexpr T As() const noexcept { return static_cast<T>(Topology); }

        Topology_t Topology;
    };

    struct CommandListType {
        template<ExplicitConvertible<CommandListType_t> T>
        [[nodiscard]] constexpr T As() const noexcept { return static_cast<T>(Type); }

        CommandListType_t Type;
    };

    struct ResourceState {
        template<ExplicitConvertible<ResourceState_t> T>
        [[nodiscard]] constexpr T As() const noexcept { return static_cast<T>(States); }

        ResourceState_t States;
    };

    struct TransitionBarrierSpecification {
        TransitionBarrierSpecification(
            ResourceState newState, 
            bool flushBarriers = false, 
            uint32_t subresource = ALL_SUBRESOURCES) noexcept
            :
            NewState(newState),
            Subresource(subresource),
            FlushBarriers(flushBarriers)
        {}

        ResourceState NewState;
        bool FlushBarriers;
        uint32_t Subresource;
    };
}