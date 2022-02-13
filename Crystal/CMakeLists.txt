set(PROJECT_NAME Crystal)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "ComputeMipMaps.h"
    "ComputeTechnique.h"
    "Core/Application.h"
    "Core/ECS/Component.h"
    "Core/ECS/Entity.h"
    "Core/Exceptions/CrystalException.h"
    "Core/FileSystem/FileSystem.h"
    "Core/Input/Keyboard.h"
    "Core/Input/Mouse.h"
    "Core/InstructionSet/CpuInfo.h"
    "Core/InstructionSet/InstructionSet.h"
    "Core/Lib/CrystalTypes.h"
    "Core/Lib/FixedString.h"
    "Core/Lib/ThreadSafeQueue.h"
    "Core/Lib/type_traits.h"
    "Core/Logging/Logger.h"
    "Core/Logging/LogLevels.h"
    "Core/Logging/Sink.h"
    "Core/Math/Common.h"
    "Core/Math/Matrix.h"
    "Core/Math/Quaternion.h"
    "Core/Math/Rectangle.h"
    "Core/Math/RNG.h"
    "Core/Math/Transform.h"
    "Core/Math/Vector2.h"
    "Core/Math/Vector3.h"
    "Core/Math/Vector4.h"
    "Core/Memory/MemoryConstants.h"
    "Core/Time/CrystalTimer.h"
    "Core/Time/Time.h"
    "Graphics/Camera.h"
    "Graphics/Graphics.h"
    "Graphics/Material.h"
 #[[   "Graphics/Mesh.h"
    "Graphics/Scene.h"]]
    "Graphics/Types/Types.h"
    "Graphics/Viewport.h"
    "Networking/NamedPipeClient.h"
    "Platform/Windows/CrystalWindow.h"
    "Platform/Windows/MessageBox.h"
    "Platform/Windows/Types.h"
    "RHI/Buffer.h"
    "RHI/CommandContext.h"
    "RHI/D3D12/D3D12Buffer.h"
    "RHI/D3D12/D3D12CommandContext.h"
    "RHI/D3D12/D3D12CommandQueue.h"
    "RHI/D3D12/D3D12Core.h"
    "RHI/D3D12/D3D12DescriptorHeap.h"
    "RHI/D3D12/D3D12DynamicDescriptorHeap.h"
    "RHI/D3D12/D3D12PipelineState.h"
    "RHI/D3D12/D3D12RenderTarget.h"
    "RHI/D3D12/D3D12RootSignature.h"
    "RHI/D3D12/D3D12SwapChain.h"
    "RHI/D3D12/D3D12Texture.h"
    "RHI/D3D12/D3D12VertexType.h"
    "RHI/D3D12/LinearAllocator.h"
    "RHI/D3D12/Managers/TextureManager.h"
    "RHI/D3D12/Utils/D3D12Exception.h"
    "RHI/D3D12/Utils/d3dx12.h"
    "RHI/D3D12/Utils/ResourceStateTracker.h"
    "RHI/PipelineState.h"
    "RHI/RenderTarget.h"
    "RHI/RHICore.h"
    "RHI/RootSignature.h"
    "RHI/SwapChain.h"
    "RHI/Texture.h"
    "RHI/VertexTypes.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "ComputeMipMaps.cpp"
    "ComputeTechnique.cpp"
    "Core/Application.cpp"
    "Core/Exceptions/CrystalException.cpp"
    "Core/FileSystem/FileSystem.cpp"
    "Core/Input/Keyboard.cpp"
    "Core/Input/Mouse.cpp"
    "Core/InstructionSet/CpuInfo.cpp"
    "Core/InstructionSet/InstructionSet.cpp"
    "Core/Logging/ManagedLoggerSink.cpp"
    "Core/Logging/ManagedLoggerSink.h"
    "Core/Math/MathFunctions.h"
    "Core/Math/Quaternion.cpp"
    "Core/Math/Transform.cpp"
    "Core/Math/Vector2.cpp"
    "Core/Math/Vector3.cpp"
    "Core/Math/Vector4.cpp"
    "Core/Utils/StringUtils.h"
    "Crystal.cpp"
    "Graphics/Camera.cpp"
    "Graphics/Graphics.cpp"
    "Graphics/Material.cpp"
   #[[ "Graphics/Mesh.cpp"
    "Graphics/Scene.cpp"]]
    "Networking/NamedPipeClient.cpp"
    "Platform/Windows/Window.cpp"
    "Platform/Windows/Window.h"
    "RHI/D3D12/D3D12Buffer.cpp"
    "RHI/D3D12/D3D12CommandContext.cpp"
    "RHI/D3D12/D3D12CommandQueue.cpp"
    "RHI/D3D12/D3D12Core.cpp"
    "RHI/D3D12/D3D12DescriptorHeap.cpp"
    "RHI/D3D12/D3D12DynamicDescriptorHeap.cpp"
    "RHI/D3D12/D3D12PipelineState.cpp"
    "RHI/D3D12/D3D12RenderTarget.cpp"
    "RHI/D3D12/D3D12RootSignature.cpp"
    "RHI/D3D12/D3D12SwapChain.cpp"
    "RHI/D3D12/D3D12Texture.cpp"
    "RHI/D3D12/LinearAllocator.cpp"
    "RHI/D3D12/Managers/TextureManager.cpp"
    "RHI/D3D12/Utils/D3D12Exception.cpp"
    "RHI/D3D12/Utils/ResourceStateTracker.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

 #[[add_subdirectory(Extern/Assimp)]]

################################################################################
# Target
################################################################################
 add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

 target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_INCLUDE_CURRENT_DIR})
 target_include_directories(${PROJECT_NAME} PRIVATE ../${CMAKE_INCLUDE_CURRENT_DIR})

set(ROOT_NAMESPACE Crystal)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        VS_GLOBAL_KEYWORD
        "Win32Proj"
)
 set_target_properties(
         ${PROJECT_NAME}
         PROPERTIES
         INTERPROCESS_OPTIMIZATION_RELEASE
         "TRUE"
    )

################################################################################
# Include directories
################################################################################
 message(${CMAKE_CURRENT_SOURCE_DIR})
 message(${CMAKE_CURRENT_SOURCE_DIR}/Extern/rangeV3)
 message(${CMAKE_CURRENT_SOURCE_DIR}/Extern/Assimp/Include)
 target_include_directories(
         ${PROJECT_NAME} PUBLIC
         "${CMAKE_CURRENT_SOURCE_DIR}"
         "${CMAKE_CURRENT_SOURCE_DIR}/Extern/Assimp/Include"
         "${CMAKE_CURRENT_SOURCE_DIR}/Extern/DirectXTex"
    )

################################################################################
# Compile definitions
################################################################################
#[[if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")]]
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
            "WIN32;"
            "_LIB;"
            "UNICODE;"
            "_UNICODE"
    )
#[[endif()]]

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /arch:AVX2
            >
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /MP;
            /std:c++latest;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++latest;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()

    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
        )
    endif()
endif()

