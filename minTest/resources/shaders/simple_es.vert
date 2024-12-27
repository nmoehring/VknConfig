#version 460 core
precision highp int;

layout(location = 0) in ivec2 voxels;
out vec4 v_color;

void main() {
    //Draw line across 
    //Screen is curved display of tiles, each tile has a center position and a normal
    //Screen is a point, every tile has the same position, and every normal points from the same coordinate
    //Works because of curvature of screen. The actual screen used to render will have definite size.
    //There will be 2 screen points, so that VR is possible, and some kind of blend will need to happen for
    //2D viewing. VR may feature some kind of voronoi processing of voxel data rather than a simple projection
    //on to an 8-bit style low-res screen, and then blending isn't needed either.

    //Different geometries can have different base voxel scales
    //Rock bottom: 1 giant textured plane
    //"Deep earth": 1 cubic yard/foot voxels (unless encountered)
    //Any surfaces: cubic inch voxel
    //Characters: 1/4 cubic inch voxels

    //Voxel layers:
    //  layer
    // 
    // Useful vectors:
    // Force: 12-bit
    // Velocity: 12-bit
    // rotation (possibly with quaternions, maybe): 6-bit

    // Useful scalars:
    // charge-number 2 bit
    // roughness 2 bit
    // is center-of-mass 1 bit
    // material 4 bit

    //Wind-density (combined flows) 5 bits 

    //32-bits for autonomy

    //universals:
    //unit-drag 
    //humidity
    //ambient temp
    //ambient air pressure

    //Smallest voxels may not need position, just material (color provided by material)
    // Unpack the data 
    //6 bits for top-level 16x16 "quadrants"
    int quadrant = voxels[0]>>26;
    //8 bits per axis
    int x = (voxels[0]>>18)&0xFF;
    int y = (voxels[0]>>10)&0xFF;
    int z = voxels[0]>>2&0x3FF;
    //2 bits for voxel tier
    int tier = voxels[0]&0x3;

    //4 bits for material (16 materials, alloys possible) (stone, dirt, water, wind)
    //      (flesh = water+leaf, bone = water+wood, wood = water+dirt, leaf = water+air)
    //      (cloud = water+air)
    int material = (voxels[1]>>28)&0xF
    // 1 bit for is center of mass
    bool isCenterOfMass = (voxels[1]>>27)&0x1
    // 2 bits for charge number
    int chargeNumber = (voxels[1]>>26)&0x1
    // 4 bits for roughness
    int roughness = (voxels[1]>>22)&0x7
    // 4 bits for heat
    int heat = (voxels[1]>>18)&0x7
    // 5-bits for neighbor bonds (14 neighbors)
    int bonds = (voxels[1]>>13)&0x1F

    // Convert to float for position 
    vec3 position = vec3(float(x), float(y), float(z))/1023.0; 
    // Convert color to vec4 
    v_color = vec4(float((color>>24)&0xFF)/255.0, float((color>>16)&0xFF)/255.0, float((color>>8)&0xFF)/255.0, float(color&0xFF)/255.0);
    gl_Position = vec4(position, 1.0);
}