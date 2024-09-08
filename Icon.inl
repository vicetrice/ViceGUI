#pragma once

#include "Icon.hpp"

namespace Vicetrice
{
    template<typename T>
    Icon<T>::Icon(std::vector<T> &Vertices)
        : m_vertices(std::move(Vertices))
    {
    }

    template<typename T>
    void Icon<T>::OnClick(void(*func)())
    {
       
    }

    template<typename T>
    void Icon<T>::AddToContext(std::vector<float>& ContextVertices, std::vector<unsigned int>& Indices, unsigned int IconNumber)
    {
       
    }

    template<typename T>
    IconType Icon<T>::Icontype()
    {
        return IconType::STATICTEXTICON;  // Retorna el tipo de ícono (modificable)
    }
}
