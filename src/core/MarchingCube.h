#pragma once

#include <vector>

#include "cinder/app/App.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/gl.h"

#include "./marching_cube_lookup.h"
#include "./util.h"

#define MARCHING_CUBE_GROUP_SIZE 8
#define CLEAR_GROUP_SIZE 1024

using namespace ci;
using namespace ci::app;

struct Grid {
    vec4 position;
    vec4 normal;
};

typedef std::shared_ptr<class MarchingCube> MarchingCubeRef;

class MarchingCube {
public:
    MarchingCube();
    ~MarchingCube();

    MarchingCubeRef size(float s);

    void setup(const int resolution);
    void update(GLuint particle_buffer, int num_items, float threshold = 0.5f);
    void render();
    void renderDensity();

    void updateField(float time, float scale);

    static MarchingCubeRef create() { return std::make_shared<MarchingCube>(); }

private:
    void prepareGridBuffer();
    void prepareDensityParticles();
    void prepareBuffers();
    void compileShaders();

    void clearDensity();

    void runClearProg();
    void runBinDensityProg(GLuint particle_buffer, int num_items);
    void runNormalizeDensityProg(const ivec3 thread);
    void runMarchingCubeProg(float threshold, const ivec3 thread);

    MarchingCubeRef thisRef() { return std::make_shared<MarchingCube>(*this); }

    int count_;
    int resolution_;
    float size_;

    gl::GlslProgRef bin_density_prog_, normalize_density_prog_;
    gl::GlslProgRef marching_cube_prog_, clear_prog_, render_prog_, render_density_prog_;

    std::vector<Grid> grids_;
    std::vector<int> indices_;
    std::vector<ivec4> particles_;

    gl::SsboRef grid_buffer_, index_buffer_, particle_buffer_, density_buffer_;
    gl::SsboRef cube_edge_flags_buffer_, triangle_connection_table_buffer_;
    gl::VboRef grid_ids_vbo_, particle_ids_vbo_;
    gl::VaoRef grid_attributes_, particle_attributes_;
};
