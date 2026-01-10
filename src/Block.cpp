#include "Block.h"

constexpr float blockSize = 0.2f;
constexpr float sx = blockSize / 2.0f;
constexpr float sy = blockSize / 2.0f;
constexpr float sz = blockSize / 2.0f;

constexpr float w = 1.0f / 3.0f;
constexpr float h = 1.0f / 2.0f;

constexpr float padTexel = 1.0f;

constexpr float atlasWidth = 48.0f;
constexpr float atlasHeight = 32.0f;

constexpr float pu = padTexel / atlasWidth * 0.5f;
constexpr float pv = padTexel / atlasHeight * 0.5f;

const std::array<FaceVertices, 6> FaceData::s_Faces = {{
    // Back face (-Z)
    {{{
        {-sx, -sy, -sz}, {sx, sy, -sz}, {sx, -sy, -sz},
        {sx, sy, -sz}, {-sx, -sy, -sz}, {-sx, sy, -sz}
    }},
    {0.0f, 0.0f, -1.0f},
    {{
        {2*w+pu, 2*h-pv}, {3*w-pu, 1*h+pv}, {3*w-pu, 2*h-pv},
        {3*w-pu, 1*h+pv}, {2*w+pu, 2*h-pv}, {2*w+pu, 1*h+pv}
    }}},
    
    // Front face (+Z)
    {{{
        {-sx, -sy, sz}, {sx, -sy, sz}, {sx, sy, sz},
        {sx, sy, sz}, {-sx, sy, sz}, {-sx, -sy, sz}
    }},
    {0.0f, 0.0f, 1.0f},
    {{
        {1*w+pu, 1*h+pv}, {2*w-pu, 1*h+pv}, {2*w-pu, 0*h+pv},
        {2*w-pu, 0*h+pv}, {1*w+pu, 0*h+pv}, {1*w+pu, 1*h+pv}
    }}},
    
    // Left face (-X)
    {{{
        {-sx, sy, sz}, {-sx, sy, -sz}, {-sx, -sy, -sz},
        {-sx, -sy, -sz}, {-sx, -sy, sz}, {-sx, sy, sz}
    }},
    {-1.0f, 0.0f, 0.0f},
    {{
        {3*w-pu, 0*h+pv}, {2*w+pu, 0*h+pv}, {2*w+pu, 1*h-pv},
        {2*w+pu, 1*h-pv}, {3*w-pu, 1*h-pv}, {3*w-pu, 0*h+pv}
    }}},
    
    // Right face (+X)
    {{{
        {sx, sy, sz}, {sx, -sy, -sz}, {sx, sy, -sz},
        {sx, -sy, -sz}, {sx, sy, sz}, {sx, -sy, sz}
    }},
    {1.0f, 0.0f, 0.0f},
    {{
        {1*w+pu, 0*h+pv}, {0*w+pu, 1*h-pv}, {0*w+pu, 0*h+pv},
        {0*w+pu, 1*h-pv}, {1*w+pu, 0*h+pv}, {1*w+pu, 1*h-pv}
    }}},
    
    // Bottom face (-Y)
    {{{
        {-sx, -sy, -sz}, {sx, -sy, -sz}, {sx, -sy, sz},
        {sx, -sy, sz}, {-sx, -sy, sz}, {-sx, -sy, -sz}
    }},
    {0.0f, -1.0f, 0.0f},
    {{
        {0*w+pu, 1*h+pv}, {1*w-pu, 1*h+pv}, {1*w-pu, 2*h-pv},
        {1*w-pu, 2*h-pv}, {0*w+pu, 2*h-pv}, {0*w+pu, 1*h+pv}
    }}},
    
    // Top face (+Y)
    {{{
        {-sx, sy, -sz}, {sx, sy, sz}, {sx, sy, -sz},
        {sx, sy, sz}, {-sx, sy, -sz}, {-sx, sy, sz}
    }},
    {0.0f, 1.0f, 0.0f},
    {{
        {1*w+pu, 1*h+pv}, {2*w-pu, 2*h-pv}, {2*w-pu, 1*h+pv},
        {2*w-pu, 2*h-pv}, {1*w+pu, 1*h+pv}, {1*w+pu, 2*h-pv}
    }}}
}};
