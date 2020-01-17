//NEON_PLANE_CC
#include "neon_plane.h"

namespace neon {
   wall::wall()
      : size_(7)
      , world_(1.0f)
      , rotation_(0.0f)
	{
	};

	bool wall::create(const string& texturePath, const string& normalMapPath){
		if (!program_.create("assets/Wall/wall_vertex_shader.txt",
									"assets/Wall/wall_fragment_shader.txt")) {
			return false;
		}

		if (!texture_.create(texturePath)) {
			return false;
		}

		if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		if (!normalMap_.create(normalMapPath)) {
			return false;
		}
      vertex vertices[] = {
         //side 1, triangle 1
         { glm::vec3(-size_,   size_,   0),   glm::vec2(0.0f,1.0f)},
         { glm::vec3(-size_,  -size_,   0),   glm::vec2(0.0f,0.0f)},
         { glm::vec3( size_,  -size_,   0),   glm::vec2(1.0f,0.0f)},
         { glm::vec3( size_,   size_,   0),   glm::vec2(1.0f,1.0f)},
      };

      dynamic_array<uint32> indices;
      indices.push_back(0);
      indices.push_back(1);
      indices.push_back(3);
      indices.push_back(1);
      indices.push_back(3);
      indices.push_back(2);

      if (!index_.create(sizeof(uint32) * (int)indices.size(),
                         GL_UNSIGNED_INT, indices.data())) {
         return false;
      }


		GLint position_location_ = program_.get_attrib_location("position");
		GLint texcoords_location_ = program_.get_attrib_location("texcoord");
      GLint normal_location_ = program_.get_attrib_location("normal");
      GLint tangent_location_ = program_.get_attrib_location("tangent");
      GLint bitangent_location_ = program_.get_attrib_location("bitangent");
		format_.add_attribute(position_location_, 3, GL_FLOAT, false);
		format_.add_attribute(texcoords_location_, 2, GL_FLOAT, false);
      format_.add_attribute(normal_location_, 3, GL_FLOAT, false);
      format_.add_attribute(tangent_location_, 3, GL_FLOAT, false);
      format_.add_attribute(bitangent_location_, 3, GL_FLOAT, false);



      glm::vec3 edge1 = vertices[1].position_ - vertices[0].position_;
      glm::vec3 edge2 = vertices[2].position_ - vertices[0].position_;
      glm::vec2 deltaUV1 = vertices[1].texcoords_ - vertices[0].texcoords_;
      glm::vec2 deltaUV2 = vertices[2].texcoords_ - vertices[0].texcoords_;

      float f = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y );

      tangent_.x = f * ( deltaUV2.y * edge1.x - deltaUV1.y * edge2.x );
      tangent_.y = f * ( deltaUV2.y * edge1.y - deltaUV1.y * edge2.y );
      tangent_.z = f * ( deltaUV2.y * edge1.z - deltaUV1.y * edge2.z );
      tangent_ = glm::normalize(tangent_);

      bitangent_.x = f * ( -deltaUV2.x * edge1.x + deltaUV1.x * edge2.x );
      bitangent_.y = f * ( -deltaUV2.x * edge1.y + deltaUV1.x * edge2.y );
      bitangent_.z = f * ( -deltaUV2.x * edge1.z + deltaUV1.x * edge2.z );
      bitangent_ = glm::normalize(bitangent_);

      for (auto& vertex : vertices){
         vertex.bitangent = bitangent_;
         vertex.tangent = tangent_;
      }

      if (!vbo_.create(sizeof(vertices), &vertices))
      {
         return false;
      }


		return true;
	}
   void wall::render(fps_camera camera)
   {
      GLenum err = GL_NO_ERROR;

      glEnable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);
      glFrontFace(GL_CW);

      program_.bind();
      program_.set_uniform_mat4("projection", camera.projection_);
      program_.set_uniform_mat4("view", camera.view_);
      program_.set_uniform_mat4("world", world_);
      program_.set_uniform_vec3("normal", normal_);

      program_.set_uniform_vec3("lightDir", light_->direction_);
      program_.set_uniform_vec3("lightColor", light_->colour_);
      program_.set_uniform_float("lightPower", light_->power_);

      program_.set_uniform_int("diffuse", 0);
      program_.set_uniform_int("normalMap", 1);

      vbo_.bind();
      index_.bind();
      format_.bind();

      texture_.bind(0);
      normalMap_.bind(1);
      sampler_.bind(0);
      sampler_.bind(1);

      index_.render(GL_TRIANGLES, 1, 6);
   }
   void wall::update(const time& deltaTime)
   {
      rotation_ = deltaTime.as_seconds();
      world_ = glm::rotate(world_, rotation_, glm::vec3(-1.0f, 1.0f, 0.0f));
      normal_.x = world_[2].x;
      normal_.y = world_[2].y;
      normal_.z = world_[2].z;
   }
}