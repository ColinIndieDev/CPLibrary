#include "Block.h"

float blockSize = 0.2f;
float sx = blockSize / 2.0f;
float sy = blockSize / 2.0f;
float sz = blockSize / 2.0f;

float w = 1.0f / 3.0f;
float h = 1.0f / 2.0f;

const std::array<FaceVertices, 6> FaceData::s_Faces = {{
    // Back face (-Z)
    {{{
        {-sx, -sy, -sz}, {sx, sy, -sz}, {sx, -sy, -sz},
        {sx, sy, -sz}, {-sx, -sy, -sz}, {-sx, sy, -sz}
    }},
    {0.0f, 0.0f, -1.0f},
    {{
        {2*w, 2*h}, {3*w, 1*h}, {3*w, 2*h},
        {3*w, 1*h}, {2*w, 2*h}, {2*w, 1*h}
    }}},
    
    // Front face (+Z)
    {{{
        {-sx, -sy, sz}, {sx, -sy, sz}, {sx, sy, sz},
        {sx, sy, sz}, {-sx, sy, sz}, {-sx, -sy, sz}
    }},
    {0.0f, 0.0f, 1.0f},
    {{
        {1*w, 1*h}, {2*w, 1*h}, {2*w, 0*h},
        {2*w, 0*h}, {1*w, 0*h}, {1*w, 1*h}
    }}},
    
    // Left face (-X)
    {{{
        {-sx, sy, sz}, {-sx, sy, -sz}, {-sx, -sy, -sz},
        {-sx, -sy, -sz}, {-sx, -sy, sz}, {-sx, sy, sz}
    }},
    {-1.0f, 0.0f, 0.0f},
    {{
        {3*w, 0*h}, {2*w, 0*h}, {2*w, 1*h},
        {2*w, 1*h}, {3*w, 1*h}, {3*w, 0*h}
    }}},
    
    // Right face (+X)
    {{{
        {sx, sy, sz}, {sx, -sy, -sz}, {sx, sy, -sz},
        {sx, -sy, -sz}, {sx, sy, sz}, {sx, -sy, sz}
    }},
    {1.0f, 0.0f, 0.0f},
    {{
        {1*w, 0*h}, {0*w, 1*h}, {0*w, 0*h},
        {0*w, 1*h}, {1*w, 0*h}, {1*w, 1*h}
    }}},
    
    // Bottom face (-Y)
    {{{
        {-sx, -sy, -sz}, {sx, -sy, -sz}, {sx, -sy, sz},
        {sx, -sy, sz}, {-sx, -sy, sz}, {-sx, -sy, -sz}
    }},
    {0.0f, -1.0f, 0.0f},
    {{
        {0*w, 1*h}, {1*w, 1*h}, {1*w, 2*h},
        {1*w, 2*h}, {0*w, 2*h}, {0*w, 1*h}
    }}},
    
    // Top face (+Y)
    {{{
        {-sx, sy, -sz}, {sx, sy, sz}, {sx, sy, -sz},
        {sx, sy, sz}, {-sx, sy, -sz}, {-sx, sy, sz}
    }},
    {0.0f, 1.0f, 0.0f},
    {{
        {1*w, 1*h}, {2*w, 2*h}, {2*w, 1*h},
        {2*w, 2*h}, {1*w, 1*h}, {1*w, 2*h}
    }}}
}};
