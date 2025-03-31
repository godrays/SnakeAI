//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

// Project includes
// External includes
#include <aix.hpp>
// System includes


namespace aix::ext
{

static void serializeModule(const aix::nn::Module& module, std::vector<float>& data)
{
    const auto params = module.parameters();
    size_t totalElements = 0;
    for (const auto& [paramName, param] : params)
    {
        if (sizeof(float) != aix::Device::dataTypeSize(param.dataType()))
        {
            throw std::out_of_range("Parameter type does not match: " + paramName);
        }
        totalElements += param.value().size();
    }
    data.reserve(totalElements);

    for (const auto& [paramName, param] : params)
    {
        const auto& value = param.value();
        const auto ptr = static_cast<const float*>(value.data());
        data.insert(data.end(), ptr, ptr + value.size());
    }
}


static void deserializeModule(const aix::nn::Module& module, const std::vector<float>& data)
{
    auto params = module.parameters();
    size_t offset = 0;

    for (auto& [paramName, param] : params)
    {
        auto& value = param.value();
        const size_t numElements = value.size();

        if (sizeof(float) != aix::Device::dataTypeSize(param.dataType()))
        {
            throw std::out_of_range("Parameter type does not match: " + paramName);
        }

        // Check if there’s enough data to deserialize this parameter
        if (offset + numElements > data.size())
        {
            throw std::out_of_range("Not enough data to deserialize parameter: " + paramName);
        }

        // Copy the parameter values from data to the parameter’s memory
        auto* dest = static_cast<float*>(value.data());
        std::copy(data.begin() + offset, data.begin() + offset + numElements, dest);
        offset += numElements;
    }

    // Ensure all data was used
    if (offset != data.size())
    {
        throw std::runtime_error("Extra data remaining after deserialization");
    }
}

} // namespace aix::ext
