// neon_graphics.cc

#include "neon_graphics.h"
#include "DirectionalLight.h"
#include <cassert>

//#define STB_IMAGE_IMPLEMENTATION // Kill this define because idk why
#include "stb_image.h"

namespace neon {
   
   // vertex Buffer
   vertex_buffer::vertex_buffer()
      : id_(0)
   {
   }

   bool vertex_buffer::create(const int size, const void* data) {
      if (is_valid())
      {
         return false;
      }

      glGenBuffers(1, &id_);
      glBindBuffer(GL_ARRAY_BUFFER, id_);
      glBufferData(GL_ARRAY_BUFFER,
                   size,
                   data,
                   GL_STATIC_DRAW);

      GLenum err = glGetError();
      return err == GL_NO_ERROR;;
   }

   void vertex_buffer::update(const int size, const void* data) {
      if (!is_valid())
      {
         return;
      }

      glBindBuffer(GL_ARRAY_BUFFER, id_);
      glBufferData(GL_ARRAY_BUFFER,
                   size,
                   data,
                   GL_STATIC_DRAW);
   }

   void vertex_buffer::destroy() {
      if (!is_valid()) {
         return;
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDeleteBuffers(1, &id_);
      id_ = 0;
   }

   bool vertex_buffer::is_valid() const {
      return id_ != 0;
   }

   void vertex_buffer::bind() const {
      glBindBuffer(GL_ARRAY_BUFFER, id_);
   }

   void vertex_buffer::render(GLenum primitive, int start, int count)
   {
      glDrawArrays(primitive, start, count);
   }
   
   // !vertex buffer
   
   // index_buffer

   index_buffer::index_buffer()
      : id_(0)
      , type_(GL_UNSIGNED_SHORT)
   {
   }

   bool index_buffer::create(const int size, const GLenum type, const void* data)
   {
      if (is_valid()) {
         return false;
      }

      type_ = type;

      glGenBuffers(1, &id_);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

      GLenum error = glGetError();
      return error == GL_NO_ERROR;
   }
   void index_buffer::update(const int size, const void* data)
   {}
   void index_buffer::destroy()
   {
      if (!is_valid()){
         return;
      }

      glDeleteBuffers(1, &id_);
      id_ = 0;
   }

   bool index_buffer::is_valid() const
   {
      return id_ != 0;
   }
   void index_buffer::bind() const
   {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
   }

   void index_buffer::render(GLenum primitive, int start, int count) {
      glDrawElements(primitive, count, type_, (const void *)nullptr);
   }

   // !index_buffer


   namespace {
      GLuint create_shader(GLenum type, const char* source) {
         GLuint id = glCreateShader(type);
         glShaderSource(id, 1, &source, nullptr);
         glCompileShader(id);
         return id;
      }
   
      GLuint create_program(GLuint vid, GLuint fid) {
         GLuint id = glCreateProgram();
         glAttachShader(id, vid);
         glAttachShader(id, fid);
         glLinkProgram(id);
   
         GLint status = 0;
         glGetProgramiv(id, GL_LINK_STATUS, &status);
         if (status == GL_FALSE) {
            char vsh_err[1024] = {};
            char fsh_err[1024] = {};
            char sh_err[1024] = {};
   
            glGetShaderInfoLog(vid, sizeof(vsh_err), nullptr, vsh_err);
            glGetShaderInfoLog(fid, sizeof(fsh_err), nullptr, fsh_err);
            glGetProgramInfoLog(id, sizeof(sh_err), nullptr, sh_err);

            assert(false);
            
            glDeleteProgram(id);
            id = 0;
         }

         glDeleteShader(vid);
         glDeleteShader(fid);
         
         return id;
      }
   } // !anon

   // shader_program

   shader_program::shader_program()
      :id_(0)
   {

   }

   bool shader_program::create(const string& vertex_shader_filename,
                               const string& fragment_shader_filename)
   {
      if (is_valid()) {
         return false;
      }

      dynamic_array<uint8> vertex_shader_file_content;
      if (!file_system::read_file_content(vertex_shader_filename,
                                          vertex_shader_file_content))
      {
         return false;
      }
      vertex_shader_file_content.push_back(0);
      const char* vertex_shader_source = (const char*)vertex_shader_file_content.data();

      dynamic_array<uint8> fragment_shader_file_content;
      if (!file_system::read_file_content(fragment_shader_filename,
                                          fragment_shader_file_content))
      {
         return false;
      }
      fragment_shader_file_content.push_back(0);
      const char* fragment_shader_source = (const char*)fragment_shader_file_content.data();
      
      GLuint vid = create_shader(GL_VERTEX_SHADER, vertex_shader_source); //vertex shader id
      GLuint fid = create_shader(GL_FRAGMENT_SHADER, fragment_shader_source); //fragment shader id
      id_ = create_program(vid, fid);

      GLenum error = glGetError();
      return error == GL_NO_ERROR;
   }
   void shader_program::destroy() {
      if (!is_valid()) {
         return;
      }

      glDeleteProgram(id_);
      id_ = 0;
   }


   GLint shader_program::get_attrib_location(const string& name) const {
      bind();
      return glGetAttribLocation(id_, name.c_str());
   }
   GLint shader_program::get_uniform_location(const string& name) const {
      bind();
      return glGetUniformLocation(id_, name.c_str());
   }


   bool shader_program::set_uniform_mat4(const string& name, const glm::mat4& value) {
      GLuint location = get_uniform_location(name);
      if (location == -1) {
         return false;
      }

      glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

      return true;
   }
   bool shader_program::set_uniform_vec4(const string& name, const glm::vec4& value) {
      GLuint location = get_uniform_location(name);
      if (location == -1){
         return false;
      }

      glUniform4fv(location, 1, glm::value_ptr(value));

      return true;
   }

   bool shader_program::set_uniform_vec3(const string& name, const glm::vec3& value) {
      GLuint location = get_uniform_location(name);
      if (location == -1) {
         return false;
      }

      glUniform3fv(location, 1, glm::value_ptr(value));

      return true;
   }

   bool shader_program::set_uniform_float(const string& name, const GLfloat value) {
      GLuint location = get_uniform_location(name);
      if (location == -1) {
         return false;
      }
      glUniform1f(location, value);

      return true;
   }

   bool shader_program::set_uniform_int(const string& name, const GLint value)
   {
      GLuint location = get_uniform_location(name);
      if (location == -1) {
         return false;
      }
      glUniform1i(location, value);

      return true;
   }

   bool shader_program::is_valid() const {
      return id_ != 0;
   }
   void shader_program::bind() const {
      glUseProgram(id_);
   }
   // !shader_program

   // vertex_format

   vertex_format::attribute::attribute()
      : index_(-1)
      , size_(0)
      , type_(0)
      , normalized_(false)
      , offset_(0)
   {

   }

   vertex_format::vertex_format()
      : stride_(0)
      , attribute_count_(0)
      , attributes_{}
   {

   }

   namespace {
      uint32 size_of_gl_type(GLenum type) {
         switch (type) {
         case GL_FLOAT:
            return sizeof(GLfloat);
         case GL_UNSIGNED_BYTE:
            return sizeof(uint8);
         }
         assert(false);
         return 0;
      }
   } // !anon


   void vertex_format::add_attribute(const int32 index, const uint32 size,
                                     const GLenum type, const bool normalized)
   {
      assert(attribute_count_ < ATTRIBUTE_COUNT);
      uint32 at = attribute_count_++;
      attributes_[at].index_ = index;
      attributes_[at].size_ = size;
      attributes_[at].type_ = type;
      attributes_[at].normalized_ = normalized;
      attributes_[at].offset_ = stride_;

      stride_ += size * size_of_gl_type(type);
   }

   bool vertex_format::is_valid() const {
      return attribute_count_ > 0;
   }

   void vertex_format::bind() const {
      for (uint32 index = 0; index < ATTRIBUTE_COUNT; index++)
      {
         glDisableVertexAttribArray(index);
      }
      for (uint32 index = 0; index < attribute_count_; index++)
      {
         const attribute& attrib = attributes_[index];
         glEnableVertexAttribArray(attrib.index_);
         glVertexAttribPointer(attrib.index_,
                               attrib.size_,
                               attrib.type_,
                               attrib.normalized_,
                               stride_,
                               (const void *)attrib.offset_);
      }
   }

   // !vertex_format

   // Texture

   texture::texture()
      : id_(0)
   {

   }

   bool texture::create(const string& filename) {
      if (is_valid()) {
         return false;
      }

      dynamic_array<uint8> file_content;
      if (!file_system::read_file_content(filename, file_content)) {
         return false;
      }

      int width = 0, height = 0, components = 0;
      auto bitmap = stbi_load_from_memory(file_content.data(),
                                          (int)file_content.size(),
                                          &width,
                                          &height,
                                          &components,
                                          STBI_rgb_alpha);

      if (!bitmap) {
         return false;
      }

      type_ = GL_TEXTURE_2D; // added this to make it less hard coded

      glGenTextures(1, &id_);
      glBindTexture(type_, id_);
      glTexImage2D(type_,
                   0,
                   GL_RGBA,
                   width,
                   height,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   bitmap);

      stbi_image_free(bitmap);
      
      GLenum error = glGetError();
      return error == GL_NO_ERROR;
   }

   bool texture::create_cubemap(int width, int height, const void** data)
   {
      if (is_valid()) {
         return false;
      }

      type_ = GL_TEXTURE_CUBE_MAP;

      glGenTextures(1, &id_);
      glBindTexture(type_, id_);
      for(int index = 0; index < 6; index++){
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, // The different places we want to bind the images too comes in order so we can use index for it
                      0,
                      GL_RGBA,
                      width,
                      height,
                      0,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      data[index]); // And this gets the right image to bind
      }
      return true;
   }

   void texture::destroy() {
      if (!is_valid()) {
         return;
      }

      glDeleteTextures(1, &id_);
      id_ = 0;
   }

   bool texture::is_valid() const {
      return id_ != 0;
   }

   void texture::bind(uint32 slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(type_, id_); // change this to type_ to make it less hard coded
   }

   // !Texture

   // Sampler_state 

   sampler_state::sampler_state()
      : id_(0)
   {
   }

   bool sampler_state::create(const GLenum filter,
                              const GLenum address_mode_u,
                              const GLenum address_mode_v) {
      if (is_valid()){
         return false;
      }
      
      glGenSamplers(1, &id_);
      bind();
      glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, filter);
      glSamplerParameteri(id_, GL_TEXTURE_MAG_FILTER, filter);
      glSamplerParameteri(id_, GL_TEXTURE_WRAP_S, address_mode_u);
      glSamplerParameteri(id_, GL_TEXTURE_WRAP_T, address_mode_v);

      GLenum error = glGetError();
      return error == GL_NO_ERROR;
   }

   void sampler_state::destroy() {
      if (!is_valid()) {
         return;
      }

      glDeleteSamplers(1, &id_);
      id_ = 0;
   }

   bool sampler_state::is_valid() const {
      return id_ != 0;
   }

   void sampler_state::bind(uint32 slot){
      glBindSampler(slot, id_);
   }

   // !Sampler_state

   // Bitmap_font
   bitmap_font::bitmap_font() 
      : projection_(0)
   {

   }

   bool bitmap_font::create() {
      if (!program_.create("assets/bitmap_font_vertex_shader.txt", // I had to make corrections to this one
                           "assets/bitmap_font_fragment_shader.txt")) {
         return false;
      }

      format_.add_attribute(0, 2, GL_FLOAT, false); // xy pos
      format_.add_attribute(1, 2, GL_FLOAT, false); // UV data

      if (!buffer_.create(512, nullptr)) {
         return false;
      }

      if (!texture_.create("assets/font_8x8.png")) {
         return false;
      }

      if (!sampler_.create(GL_NEAREST,
                          GL_CLAMP_TO_EDGE,
                          GL_CLAMP_TO_EDGE))
      {
         return false;
      }

      projection_ = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);

      return true;
   }
   void bitmap_font::destroy() {

   }

   void bitmap_font::render(const float pos_x, const float pos_y, // changing these to new variable names because we want tochange them even though they are const.
                            const string& text)
   {
      constexpr int character_per_row = 16;
      const float size = 8.0f;
      const float uv1 = 1.0f / 16.0f;

      float px = pos_x; // Here we bind them to non const variables
      float py = pos_y;

      for (auto&& character : text) {
         int index = character - ' ';
         int x = index % character_per_row;
         int y = index / character_per_row + 2;

         float u = (float)x * uv1; // change this just for very slight speed optimization
         float v = (float)y * uv1;

         vertex verts[6] =
         {
            {px      , py      , u      , v      }, // Removed '+ x' and '+ y' here because they "didn't make sense";
            {px+ size, py      , u + uv1, v      },
            {px+ size, py+ size, u + uv1, v + uv1},

            {px+ size, py+ size, u + uv1, v + uv1},
            {px      , py+ size, u      , v + uv1},
            {px      , py      , u      , v      },

         };
         
         for (auto& vert : verts) {
            vertices_.push_back(vert);
         }

         px += size; // So that the next letter is placed to the left of the first one and so forth
      }
   }

   void bitmap_font::flush() {
      // note: submit the vertices from CPU to GPU
      int size = (int)( sizeof(vertex) * vertices_.size() );
      buffer_.update(size,
                     vertices_.data());

      // note: render the shit!
      glDisable(GL_DEPTH_TEST);
      //glDisable(GL_BLEND); // Check this first if errors

      program_.bind();
      program_.set_uniform_mat4("projection", projection_);
      buffer_.bind();
      format_.bind();
      texture_.bind();
      sampler_.bind();
      glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices_.size());

      vertices_.clear();
   }
   // !Bitmap_font

   // fps_camera
   fps_camera::fps_camera() // Just setting standard values
      : yaw_(0.0f)
      , pitch_(0.0f)
      , roll_(0.0f)
      , x_axis(0.0f)
      , y_axis(0.0f)
      , z_axis(0.0f)
      , position_(0.0f)
      , projection_(1.0f)
      , view_(1.0f)

   {

   }

   void fps_camera::update() {
      glm::vec3 x(1.0f, 0.0f, 0.0f); // three unit vectors for our 3 axis, probably for math
      glm::vec3 y(0.0f, 1.0f, 0.0f);
      glm::vec3 z(0.0f, 0.0f, 1.0f);

      glm::mat4 ry = glm::rotate(glm::mat4(1.0f), yaw_, y); // we always do rotation from a identity matrix otherwise your rotations would behave strangely
      
      x = ry * glm::vec4(x, 0.0f); // This is to prevent gimbal lock. All axises will stay perpendicular at all times.
      z = ry * glm::vec4(z, 0.0f); // or is it spelled Gimball?

      glm::mat4 rx = glm::rotate(glm::mat4(1.0f), pitch_, x); // Same as above

      y = rx * glm::vec4(y, 0.0f); // Again same as above
      z = rx * glm::vec4(z, 0.0f);

      view_ = glm::mat4(1.0f); //always start from a identity matrix, makes things easier.
      view_[0][0] = x.x; view_[1][0] = x.y; view_[2][0] = x.z; // I think we are just setting in our new rotations into the view matrix now.
      view_[0][1] = y.x; view_[1][1] = y.y; view_[2][1] = y.z; // Didn't know we could acess things in the matrix like this though
      view_[0][2] = z.x; view_[1][2] = z.y; view_[2][2] = z.z; // kinda neat

      view_[3][0] = -glm::dot(position_, x); // setting translation in our new view matrix so that we can move the camera
      view_[3][1] = -glm::dot(position_, y); // Not having the negative sign gave me visual bugs
      view_[3][2] = -glm::dot(position_, z);

      // note: movement helpers
      x_axis = glm::normalize(x); // Floating point error might bug this shit out, but tommi is to lazy to normalize it
      y_axis = glm::normalize(y); // doing it myself though
      z_axis = glm::normalize(z);
   }

   void fps_camera::set_perspective(float fov, float aspect, float znear, float zfar) // be advised fov in this is in radians
   {
      projection_ = glm::perspective(fov, aspect, znear, zfar);
   }

   void fps_camera::rotate_x(float amount) {
      pitch_ += amount; // rotation is hard lmao;
   }

   void fps_camera::rotate_y(float amount) {
      yaw_ += amount;
   }

   void fps_camera::forward(float amount) {
      position_ += z_axis * amount; // after rotation z_axis is always forward, so increasing it means moving forward
   }
   
   void fps_camera::sidestep(float amount) {
      position_ += x_axis * amount; // the same as above but x_axis is always to the side instead
   }
   // !fps_camera

   // controller
   controller::controller(fps_camera& camera, DirectionalLight& light, keyboard& kb, mouse& m)
      : camera_(camera)
      , light_(light)
      , keyboard_(kb)
      , mouse_(m)
   {
      
   }

   void controller::update(const time& deltatime) {
      constexpr float camera_speed = 5.0f; // Constant for cameras movement speed
      const float amount = camera_speed * deltatime.as_seconds(); // Times delta time so that it's constant

      if (keyboard_.is_down(KEYCODE_W)) { // forward
         camera_.forward(-amount);
      }

      if (keyboard_.is_down(KEYCODE_S)) { // backward
         camera_.forward(amount);
      }

      if (keyboard_.is_down(KEYCODE_D)) { // right
         camera_.sidestep(amount);
      }
      if (keyboard_.is_down(KEYCODE_A)) { // left
         camera_.sidestep(-amount);
      }

      if (mouse_.is_down(MOUSE_BUTTON_RIGHT)) { //Mouse controls, we only rotate the camera while holding down rmb
         glm::vec2 mouse_delta; // create a variable to store the delta of the mouse
         mouse_delta.x = (float)mouse_.x_;  // set delta to current values
         mouse_delta.y = (float)mouse_.y_;
         mouse_delta -= mouse_position_;    // minus the value from last frame

         if (fabsf(mouse_delta.x) > 0.0f)
         {
            camera_.rotate_y(glm::radians(-mouse_delta.x)); 
         }
         if (fabsf(mouse_delta.y) > 0.0f)
         {
            camera_.rotate_x(glm::radians(-mouse_delta.y));
         }
      }

      mouse_position_ = glm::vec2((float)mouse_.x_, (float)mouse_.y_);

      camera_.update();

      // Lightsource controller

      if (keyboard_.is_down(KEYCODE_UP)) {
         light_.ChangeColour(deltatime.as_seconds());
      }
      if (keyboard_.is_down(KEYCODE_DOWN)) {
         light_.ChangeColour(-deltatime.as_seconds());
      }
      if (keyboard_.is_down(KEYCODE_LEFT)) {
         light_.rotate(deltatime.as_seconds() * 10);
      }
      if (keyboard_.is_down(KEYCODE_RIGHT)) {
         light_.rotate(-deltatime.as_seconds() * 10);
      }
   }
   // !controller

   // skybox
   skybox::skybox() {}

   bool skybox::create() {
      const char* names[] = // just hardcode all the filepaths, like a boss
      {
         "assets/skybox/xpos.png",
         "assets/skybox/xneg.png",
         "assets/skybox/ypos.png",
         "assets/skybox/yneg.png",
         "assets/skybox/zpos.png",
         "assets/skybox/zneg.png",
      };

      image sides[6];
      for (int index = 0; index < 6; index++) {
         if (!sides[index].create_from_file(names[index])){ // Load all images, assert if anything fails
            assert(!"could not load cubemap image!");
            return false;
         }
      }

      const int width = sides[0].width();
      const int height = sides[0].height();
      for (int index = 1; index < 6; index++) // check that the images have the same dimensions
      {
         if (width != sides[index].width() || height != sides[index].height()) {
            assert(!"cubemap images have to be the same dimension!");
            return false;
         }
      }

      const void* faces[6]{ }; //Not sure what we are doing here
      for (int index = 0; index < 6; index++) {
         faces[index] = sides[index].data();
      }

      if (!cubemap_.create_cubemap(width, height, faces)) {
         assert(!"could not create cubemap!");
         return false;
      }

      const float Q = 1.0f; // this is to make the cube be around the camera
      const glm::vec3 vertices[] = // These are just all or vertices
      {
         // x positive
         {  Q,  Q, -Q },
         {  Q,  Q,  Q },
         {  Q, -Q,  Q },
         {  Q, -Q,  Q },
         {  Q, -Q, -Q },
         {  Q,  Q, -Q },

         // x negative
         { -Q,  Q,  Q },
         { -Q,  Q, -Q },
         { -Q, -Q, -Q },
         { -Q, -Q, -Q },
         { -Q, -Q,  Q },
         { -Q,  Q,  Q },

         // y positive
         { -Q,  Q,  Q },
         {  Q,  Q,  Q },
         {  Q,  Q, -Q },
         {  Q,  Q, -Q },
         { -Q,  Q, -Q },
         { -Q,  Q,  Q },

         // y negative
         { -Q, -Q, -Q },
         {  Q, -Q, -Q },
         {  Q, -Q,  Q },
         {  Q, -Q,  Q },
         { -Q, -Q,  Q },
         { -Q, -Q, -Q },

         // z positive
         { -Q,  Q, -Q },
         {  Q,  Q, -Q },
         {  Q, -Q, -Q },
         {  Q, -Q, -Q },
         { -Q, -Q, -Q },
         { -Q,  Q, -Q },

         // z negative
         {  Q,  Q,  Q },
         { -Q,  Q,  Q },
         { -Q, -Q,  Q },
         { -Q, -Q,  Q },
         {  Q, -Q,  Q },
         {  Q,  Q,  Q },
      };

      if (!buffer_.create(sizeof(vertices), vertices)) { // adding the vertices to the buffer
         return false;
      }

      format_.add_attribute(0, 3, GL_FLOAT, false); // creating the attribute

      if (!program_.create("assets/skybox/vertex_shader.txt",
                           "assets/skybox/fragment_shader.txt")) {
         return false;
      }

      if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE))
      {
         return false;
      }

      return true;
   }
   void skybox::destroy() {}

   void skybox::render(const fps_camera& camera){ // pretty standard render function
      glm::mat4 fixed_view = camera.view_;
      fixed_view[3][0] = 0.0f;
      fixed_view[3][1] = 0.0f;
      fixed_view[3][2] = 0.0f;

      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
      glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

      program_.bind();
      program_.set_uniform_mat4("projection", camera.projection_); // regular render stuff
      program_.set_uniform_mat4("view", fixed_view);

      buffer_.bind();
      format_.bind();
      cubemap_.bind();
      sampler_.bind();
      glDrawArrays(GL_TRIANGLES, 0, 36);
   }
   // !skybox

   // terrain

   terrain::terrain()
      :index_count_(0)
   {
   }

   bool terrain::create(const string& heightmap_filename, const string& texture_filename) {
      
      image heightmap;
      if (!heightmap.create_from_file(heightmap_filename.c_str())) {
         return false;
      }

      dynamic_array<vertex> vertices;
      const int32 width = heightmap.width();
      const int32 height = heightmap.height();
      const int32 stride = width * 4; // note: 4 channels per pixel (rgba)

      for (int32 z = 0; z < height; z++)
      {
         for (int32 x = 0; x < width; x++)
         {
            const uint32 offset = x * 4 + z * stride;
            const uint8* rgba = heightmap.data() + offset;

            vertex tmp;
            tmp.position_ = { x/2.55f - 50, rgba[1] / 25.5f - 10, z/2.55f - 50 };
            tmp.texcoord_ = { (float)x / (float)width, (float)z / (float)height };
            tmp.normal_ = {0.0f, 0.0f, 0.0f};
            vertices.push_back(tmp);
         }
      }


      format_.add_attribute(0, 3, GL_FLOAT, false); //Adding the vertex to format_
      format_.add_attribute(1, 2, GL_FLOAT, false); //TexCoord;
      format_.add_attribute(2, 3, GL_FLOAT, false); //Normals

      dynamic_array<uint32> indices;
      for (int32 z = 0; z < height - 1; z++)
      {
         for (int32 x = 0; x < width - 1; x++)
         {
            int topLeftIndexNum = z * width + x;
            int topRightIndexNum = z * width + x + 1;
            int bottomLeftIndexNum = ( z + 1 ) * width + x;
            int bottomRightIndexNum = ( z + 1 ) * width + x + 1;

            ////////////
            //Triangle 1
            indices.push_back(topLeftIndexNum);
            indices.push_back(topRightIndexNum);
            indices.push_back(bottomRightIndexNum);

            // Calculate triangle normal, add that normal to all vertices involved.
            glm::vec3 ab, ac, crossProduct;
            ab = vertices.at(topRightIndexNum).position_ - vertices.at(topLeftIndexNum).position_;
            ac = vertices.at(bottomLeftIndexNum).position_ - vertices.at(topLeftIndexNum).position_;
            crossProduct = glm::normalize(glm::cross(ac, ab));

            vertices.at(topLeftIndexNum).normal_ += crossProduct;
            vertices.at(topRightIndexNum).normal_ += crossProduct;
            vertices.at(bottomRightIndexNum).normal_ += crossProduct;

            // vertices.at(topLeftIndexNum).normal_ =;

            //Triangle 2
            indices.push_back(bottomRightIndexNum);
            indices.push_back(bottomLeftIndexNum);
            indices.push_back(topLeftIndexNum);

            // Same as above but for triangle 2

            ab = vertices.at(bottomLeftIndexNum).position_ - vertices.at(bottomRightIndexNum).position_;
            ac = vertices.at(topLeftIndexNum).position_ - vertices.at(bottomRightIndexNum).position_;
            crossProduct = glm::normalize(glm::cross(ac, ab));

            vertices.at(bottomRightIndexNum).normal_ += crossProduct;
            vertices.at(bottomLeftIndexNum).normal_ += crossProduct;
            vertices.at(topLeftIndexNum).normal_ += crossProduct;
         }
      }
      index_count_ = (int)indices.size();

      // TODO: Normalize ALL normals in vertices.
      for (auto&& vertex : vertices)
      {
         vertex.normal_ = glm::normalize(vertex.normal_);
      }

      // Creating everything
      if (!program_.create("assets/heightmap/vertex_shader.txt",
                           "assets/heightmap/fragment_shader.txt")) {
         return false;
      }

      if (!vertex_buffer_.create(sizeof(vertex) * (int)vertices.size(),
                                 vertices.data())) {
         return false;
      }
      vertices.clear();

      if (!index_buffer_.create(sizeof(uint32) * (int)indices.size(),
                                GL_UNSIGNED_INT,
                                indices.data())) {
         return false;
      }
      indices.clear();

      if (!texture_.create(texture_filename)) {
         return false;
      }
      
      if (!sampler_.create(GL_NEAREST,
                           GL_CLAMP_TO_EDGE,
                           GL_CLAMP_TO_EDGE)) {
         return false;
      }

      glm::mat4 world = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(-5.0f, -3.0f, -10.0f));

      program_.set_uniform_mat4("world", world);

      return true;
   }
   void terrain::destroy()
   {
   }

   void terrain::render()
   {
      program_.set_uniform_mat4("projection", camera_->projection_);
      program_.set_uniform_mat4("view", camera_->view_);
		program_.set_uniform_vec3("lightDir", light_->direction_);
		program_.set_uniform_vec3("lightColor", light_->colour_);
		program_.set_uniform_float("lightPower", light_->power_ / 2);

      glFrontFace(GL_CW);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      //glDisable(GL_CULL_FACE);
      program_.bind();
      vertex_buffer_.bind();
      format_.bind();
      index_buffer_.bind();
      texture_.bind();
      sampler_.bind();
      index_buffer_.render(GL_TRIANGLES, 1, index_count_);
   }

   // !terrain

   // Code from Tommi
   
   // bounding_spere

   bounding_sphere::bounding_sphere()
      : center_{}
      , radius_{}
   {
   }

   bounding_sphere::bounding_sphere(const glm::vec3& center, float radius)
      : center_(center)
      , radius_(radius)
   {
   }

   void bounding_sphere::set_center(const glm::vec3& center) {
      center_ = center;
   }

   void bounding_sphere::set_radius(const float radius) {
      radius_ = radius;
   }

   // !bounding_sphere
   // plane

   plane::plane()
      : normal_{}
      , d_{}
   {
   }

   frustum::frustum()
   {
   }

   // source: https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
   void frustum::construct_from_view_matrix(const glm::mat4& view) {
      // note: extract left plane
      planes_[plane::PLANE_LEFT].normal_ = glm::vec3(view[0][3] + view[0][0],
                                                     view[1][3] + view[1][0],
                                                     view[2][3] + view[2][0]);
      planes_[plane::PLANE_LEFT].d_ = view[3][3] + view[3][0];

      // note: extract right plane
      planes_[plane::PLANE_RIGHT].normal_ = glm::vec3(view[0][3] - view[0][0],
                                                      view[1][3] - view[1][0],
                                                      view[2][3] - view[2][0]);
      planes_[plane::PLANE_RIGHT].d_ = view[3][3] - view[3][0];

      // note: extract top plane
      planes_[plane::PLANE_TOP].normal_ = glm::vec3(view[0][3] - view[0][1],
                                                    view[1][3] - view[1][1],
                                                    view[2][3] - view[2][1]);
      planes_[plane::PLANE_TOP].d_ = view[3][3] - view[3][1];

      // note: extract bottom plane
      planes_[plane::PLANE_BOTTOM].normal_ = glm::vec3(view[0][3] + view[0][1],
                                                       view[1][3] + view[1][1],
                                                       view[2][3] + view[2][1]);
      planes_[plane::PLANE_BOTTOM].d_ = view[3][3] + view[3][1];

      // note: extract far plane
      planes_[plane::PLANE_FAR].normal_ = glm::vec3(view[0][3] - view[0][2],
                                                    view[1][3] - view[1][2],
                                                    view[2][3] - view[2][2]);
      planes_[plane::PLANE_FAR].d_ = view[3][3] - view[3][2];

      // note: extract near plane
      planes_[plane::PLANE_NEAR].normal_ = glm::vec3(view[0][2],
                                                     view[1][2],
                                                     view[2][2]);
      planes_[plane::PLANE_NEAR].d_ = view[3][2];


      for (int32 i = 0; i < plane::PLANE_COUNT; i++) {
         float length = glm::length(planes_[i].normal_);
         planes_[i].normal_ /= length;
         planes_[i].d_ /= length;
      }
   }

   // !plane

   // frustrum

   bool frustum::is_inside(const glm::vec3& point) const {
      for (int32 index = 0; index < plane::PLANE_COUNT; index++) {
         float dist = glm::dot(planes_[index].normal_, point) +
            planes_[index].d_;
         if (dist < 0.0f)
            return false;
      }

      return true;
   }

   bool frustum::is_inside(const bounding_sphere& sphere) const {
      for (int32 index = 0; index < plane::PLANE_COUNT; index++) {
         float dist = glm::dot(planes_[index].normal_, sphere.center_) +
            planes_[index].d_;
         if (dist < -sphere.radius_)
            return false;
      }

      return true;
   }

   // !frustrum
   // transform

   transform::transform()
      : origin_(0.0f, 0.0f, 0.0f)
      , position_(0.0f, 0.0f, 0.0f)
      , scale_(1.0f, 1.0f, 1.0f)
      , rotation_(0.0f, 0.0f, 0.0f)
   {
   }

   void transform::set_origin(const glm::vec3& origin) {
      origin_ = origin;
   }

   void transform::set_position(const glm::vec3& position) {
      position_ = position;
   }

   void transform::set_scale(const glm::vec3& scale) {
      scale_ = scale;
   }

   void transform::set_rotation(const glm::vec3& rotation) {
      rotation_ = rotation;
   }

   // !transform
   // renderable

   renderable::renderable()
      : program_(nullptr)
      , texture_(nullptr)
      , sampler_state_(nullptr)
      , vertex_buffer_(nullptr)
      , index_buffer_(nullptr)
      , format_(nullptr)
   {
   }

   void renderable::render(const fps_camera& camera) {
      // ...
   }

   // !renderable
   // node

   scene::node::node()
   {
   }

   scene::scene()
   {
   }

   void scene::render(const fps_camera& camera) {
      frustum frustum;
      frustum.construct_from_view_matrix(camera.view_);

      for (auto&& node : nodes_) {
         if (frustum.is_inside(node.sphere_)) {
            node.renderable_.render(camera);
         }
      }
   }

   // !node
   // !Code from Tommi

} //!neon