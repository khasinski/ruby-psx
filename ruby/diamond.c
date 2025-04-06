// Diamond gemstone model file (replacing cube)
// Note: The variable names remain "modelCube" and "modelCube1" as requested.

//
// Mesh definition:
//   - Indices 0..7: Table (top facet) vertices (an octagon) at z = +64
//   - Index 8: Center of the table at (0,0,64)
//   - Indices 9..16: Girdle vertices (the widest part) at z = 0 (an octagon with radius 128)
//   - Index 17: Bottom apex (pavilion point) at (0,0,-128)
//
SVECTOR modelCube_mesh[] = {
    // Table vertices (0-7) at z = 64
    {  64,   0,  64 },
    {  45,  45,  64 },
    {   0,  64,  64 },
    { -45,  45,  64 },
    { -64,   0,  64 },
    { -45, -45,  64 },
    {   0, -64,  64 },
    {  45, -45,  64 },
    // Table center (8)
    {   0,   0,  64 },
    // Girdle vertices (9-16) at z = 0 (radius approx. 128)
    { 128,   0,   0 },
    {  90,  90,   0 },
    {   0, 128,   0 },
    { -90,  90,   0 },
    { -128,  0,   0 },
    { -90, -90,   0 },
    {   0, -128,  0 },
    {  90, -90,   0 },
    // Bottom apex (17)
    {   0,   0, -128 }
};

//
// Normals for each vertex (SVECTOR has 4 components; the last is unused here).
// For simplicity the table vertices (and center) are given an upward normal,
// the girdle vertices get normals in the horizontal direction (computed approximately),
// and the bottom apex gets a downward normal.
//
SVECTOR modelCube_normal[] = {
    // Table vertices and center (indices 0-8)
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    // Girdle vertices (indices 9-16)
    {2365,    0,    0, 0 },                // (128,0,0)
    {1675, 1675,    0, 0 },                // (90,90,0) ~ (2365/√2)
    {   0, 2365,    0, 0 },                // (0,128,0)
    {-1675, 1675,    0, 0 },                // (-90,90,0)
    {-2365,    0,    0, 0 },                // (-128,0,0)
    {-1675,-1675,    0, 0 },                // (-90,-90,0)
    {   0,-2365,    0, 0 },                // (0,-128,0)
    {1675, -1675,    0, 0 },                // (90,-90,0)
    // Bottom apex (17)
    {  0,    0,-2365, 0 }
};

//
// Color data for each triangle vertex.
// Our model is built from 32 triangles (8 for the table, 16 for the crown, 8 for the pavilion).
// (32 triangles * 3 vertices each = 96 entries)
// Each vertex is given dark red: RGB = (139, 0, 0), with a trailing 0.
// (The values below are repeated 96 times.)
//
CVECTOR modelCube_color[] = {
    // Line 1 (8 entries)
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 2
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 3
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 4
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 5
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 6
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 7
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    // Line 8
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0
};

//
// Index array (defines the triangles).
// We build 32 triangles from the 18 vertices:
//   A. Table (top facet) – 8 triangles:
//      Each triangle uses the table center (index 8) and two adjacent table vertices (indices 0..7).
//   B. Crown facets (connecting table to girdle) – 16 triangles:
//      For each table edge (vertex i and its neighbor) use the corresponding girdle vertices.
//      Here the quadrilateral with vertices (table[i], table[i+1], girdle[i+1], girdle[i])
//      is split into two triangles: (table[i], table[i+1], girdle[i+1])
//                                  and (table[i], girdle[i+1], girdle[i]).
//      (Remember: for table vertices, index i; for girdle, vertex = i + 9. Wrap around at i = 7.)
//   C. Pavilion facets (connecting girdle to bottom apex) – 8 triangles:
//      For each adjacent pair of girdle vertices, form a triangle with the bottom apex (index 17).
//
int modelCube_index[] = {
    // A. Table face (8 triangles)
    8, 0, 1,
    8, 1, 2,
    8, 2, 3,
    8, 3, 4,
    8, 4, 5,
    8, 5, 6,
    8, 6, 7,
    8, 7, 0,
    // B. Crown facets (16 triangles)
    // For i = 0
    0, 1, 10,
    0, 10, 9,
    // For i = 1
    1, 2, 11,
    1, 11, 10,
    // For i = 2
    2, 3, 12,
    2, 12, 11,
    // For i = 3
    3, 4, 13,
    3, 13, 12,
    // For i = 4
    4, 5, 14,
    4, 14, 13,
    // For i = 5
    5, 6, 15,
    5, 15, 14,
    // For i = 6
    6, 7, 16,
    6, 16, 15,
    // For i = 7 (wraps around)
    7, 0, 9,
    7, 9, 16,
    // C. Pavilion facets (8 triangles)
    9, 10, 17,
    10, 11, 17,
    11, 12, 17,
    12, 13, 17,
    13, 14, 17,
    14, 15, 17,
    15, 16, 17,
    16, 9, 17
};

//
// Finally, the TMESH structure for the model.
// The last parameter is the number of triangles (32).
//
TMESH modelCube = {
    modelCube_mesh,
    modelCube_normal,
    0,
    modelCube_color,
    32
};

//
// Duplicate the model for modelCube1
//
SVECTOR modelCube1_mesh[] = {
    {  64,   0,  64 },
    {  45,  45,  64 },
    {   0,  64,  64 },
    { -45,  45,  64 },
    { -64,   0,  64 },
    { -45, -45,  64 },
    {   0, -64,  64 },
    {  45, -45,  64 },
    {   0,   0,  64 },
    { 128,   0,   0 },
    {  90,  90,   0 },
    {   0, 128,   0 },
    { -90,  90,   0 },
    { -128,  0,   0 },
    { -90, -90,   0 },
    {   0, -128,  0 },
    {  90, -90,   0 },
    {   0,   0, -128 }
};

SVECTOR modelCube1_normal[] = {
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {  0,    0, 2365, 0 },
    {2365,    0,    0, 0 },
    {1675, 1675,    0, 0 },
    {   0, 2365,    0, 0 },
    {-1675, 1675,    0, 0 },
    {-2365,    0,    0, 0 },
    {-1675,-1675,    0, 0 },
    {   0,-2365,    0, 0 },
    {1675, -1675,    0, 0 },
    {  0,    0,-2365, 0 }
};

CVECTOR modelCube1_color[] = {
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0,
    139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0, 139,0,0,0
};

int modelCube1_index[] = {
    8,0,1,
    8,1,2,
    8,2,3,
    8,3,4,
    8,4,5,
    8,5,6,
    8,6,7,
    8,7,0,
    0,1,10,
    0,10,9,
    1,2,11,
    1,11,10,
    2,3,12,
    2,12,11,
    3,4,13,
    3,13,12,
    4,5,14,
    4,14,13,
    5,6,15,
    5,15,14,
    6,7,16,
    6,16,15,
    7,0,9,
    7,9,16,
    9,10,17,
    10,11,17,
    11,12,17,
    12,13,17,
    13,14,17,
    14,15,17,
    15,16,17,
    16,9,17
};

TMESH modelCube1 = {
    modelCube1_mesh,
    modelCube1_normal,
    0,
    modelCube1_color,
    32
};