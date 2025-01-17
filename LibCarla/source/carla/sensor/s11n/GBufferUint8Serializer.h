// Copyright (c) 2022 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Memory.h" // 包含内存管理相关定义，例如智能指针 SharedPtr
#include "carla/sensor/RawData.h" // 包含传感器的原始数据类型定义

#include <cstdint> // 提供固定宽度的整数类型
#include <cstring> // 提供内存操作函数

namespace carla {
namespace sensor {

  class SensorData; // 前向声明 SensorData，表示通用的传感器数据接口

namespace s11n {

  /// Serializes image buffers generated by camera sensors.
  class GBufferUint8Serializer {
  public:

#pragma pack(push, 1)
    struct ImageHeader { // 表示图像的元信息，用于描述图像的宽度、高度和视场角
      uint32_t width; // 图像的宽度
      uint32_t height; // 图像的高度
      float fov_angle; // 图像的视场角
    };
#pragma pack(pop)

    constexpr static auto header_offset = sizeof(ImageHeader); // 图像数据的偏移量（字节数），对应'ImageHeader'的大小

    static const ImageHeader &DeserializeHeader(const RawData &data) {
      // 将 RawData 的起始位置解释为 ImageHeader 结构
      return *reinterpret_cast<const ImageHeader *>(data.begin());
    }

    /// @brief 序列化图像数据到缓冲区中
    /// @tparam Sensor 传感器类型
    /// @param sensor 输入的传感器对象
    /// @param bitmap 图像数据缓冲区
    /// @param ImageWidth 图像宽度
    /// @param ImageHeight 图像高度
    /// @param FovAngle 图像视场角
    /// @return 返回更新后的缓冲区
    template <typename Sensor>
    static Buffer Serialize(const Sensor &sensor, Buffer &&bitmap, 
        uint32_t ImageWidth, uint32_t ImageHeight, float FovAngle);

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

  template <typename Sensor>
  inline Buffer GBufferUint8Serializer::Serialize(const Sensor &/*sensor*/, Buffer &&bitmap, 
      uint32_t ImageWidth, uint32_t ImageHeight, float FovAngle) {
    DEBUG_ASSERT(bitmap.size() > sizeof(ImageHeader)); // 检查缓冲区大小是否足够存储图像头部信息
    ImageHeader header = { // 构造图像头部信息
      ImageWidth,
      ImageHeight,
      FovAngle
    };
    std::memcpy(bitmap.data(), reinterpret_cast<const void *>(&header), sizeof(header)); // 将头部信息拷贝到缓冲区的起始位置
    return std::move(bitmap); // 返回更新后的缓冲区
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
