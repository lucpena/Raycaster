#ifndef TEXTURES_H
#define TEXTURES_H

struct Texture {
    // Image dimensions
    size_t img_w, img_h;

    // Number of textures and size in pixels
    size_t count, size;

    // Textures storage container
    std::vector<uint32_t> img;

    Texture(const std::string filename);

    // Get the pixel (i,j) from the texture idx
    uint32_t& get(const size_t i, const size_t j, const size_t idx);

    // Retrieve one column (tex_coord) from the texture_id and scale it to the destination size
    std::vector<uint32_t> get_scaled_column(const size_t texture_id, const size_t tex_coord, const size_t column_height);
};

#endif // TEXTURES_H