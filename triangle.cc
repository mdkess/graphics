#include <iostream>
#include <vector>
#include <algorithm>

#include <string>
#include <fstream>
#include <streambuf>

#include <cmath>

#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;




GLuint vertex_buffer_object;
GLuint program;

GLuint offset_location;
GLuint time_location;

void init_vertex_buffers() {
  const float vertex_data[] = {
     0.00f,  0.75f, 0.0f, 1.0f, //position
        1.0f, 0.0f, 0.0f, 1.0f, //color

     0.75f, -0.75f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,

    -0.75f, -0.75f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
  };

  //Generate a position vector object
  glGenBuffers(1, &vertex_buffer_object);
  //Bind it as the current array buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
  //Allocate GPU memory to store the vertex buffer, and copy over the vertex positions
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
  //Unbind the buffer
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

GLuint create_shader(GLenum shader_type, const string& shader_file_str) {
  GLuint shader = glCreateShader(shader_type);

  const char* shader_file_data = shader_file_str.c_str();
  glShaderSource(shader, 1, &shader_file_data, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if(GL_FALSE == status) {
    GLint info_log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

    GLchar* info_log_str = new GLchar[info_log_length + 1];
    glGetShaderInfoLog(shader, info_log_length, NULL, info_log_str);

    const char* shader_type_str = NULL;

    switch(shader_type) {
    case GL_VERTEX_SHADER: shader_type_str = "vertex"; break;
    case GL_GEOMETRY_SHADER: shader_type_str = "geometry"; break;
    case GL_FRAGMENT_SHADER: shader_type_str = "fragment"; break;
    }

    cerr << "Compile failure in " << shader_type_str << " shader: " << info_log_str << std::endl;
    delete[] info_log_str;
  }

  return shader;
}

GLuint create_program(const vector<GLuint>& shader_list) {
  //Create the program index
  GLuint program = glCreateProgram();

  //Attach the shaders to the program
  for_each(shader_list.begin(), shader_list.end(), [&program](GLuint p) {
    glAttachShader(program, p);
  });

  //Try to link
  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  if(GL_FALSE == status) {
    GLint info_log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

    GLchar* info_log_str = new GLchar[info_log_length + 1];
    glGetProgramInfoLog(program, info_log_length, NULL, info_log_str);

    cerr << "Linker error: " << info_log_str << std::endl;
    delete[] info_log_str;
  }

  for_each(shader_list.begin(), shader_list.end(), [&program](GLuint p) {
    glDetachShader(program, p);
  });

  return program;
}

string load_file(const string& path) {
  ifstream fin(path);
  return string((istreambuf_iterator<char>(fin)), (istreambuf_iterator<char>()));
}

void initialize_program() {
  vector<GLuint> shader_list;

  string vertex_shader_str   = load_file("shaders/triangle.vert");
  string fragment_shader_str = load_file("shaders/triangle.frag");

  shader_list.push_back(create_shader(GL_VERTEX_SHADER  , vertex_shader_str));
  shader_list.push_back(create_shader(GL_FRAGMENT_SHADER, fragment_shader_str));

  program = create_program(shader_list);

  for_each(shader_list.begin(), shader_list.end(), glDeleteShader);

  //Now we can set the offset within the code
  offset_location = glGetUniformLocation(program, "offset");
  time_location = glGetUniformLocation(program, "time");
}

void calculate_offset(float& offset_x, float& offset_y) {
  const float loop_duration = 5.0f;
  const float scale = 3.14159 * 2.0f / loop_duration;

  float elapsed_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  float current_offset = fmodf(elapsed_time, loop_duration);

  offset_x = cosf(current_offset * scale) * 0.5f;
  offset_y = sinf(current_offset * scale) * 0.5f;
}

void check_errors() {
  const GLenum error = glGetError();

  if(GL_NO_ERROR != error) {
    //cout << "Error: " << error << " " << gluErrorString(error) << endl;

    cout << "Error: " << error << endl;
  }
}

void render() {

  check_errors();

  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  //Set our uniform position.
  glUniform1f(time_location, glutGet(GLUT_ELAPSED_TIME)/1000.0f);

  //Bind our position buffer as the current array buffer;
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
  // Our vertex data is at position = 0 (see vertex shader)
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  //Tell GL that we are looking at 4 floats, they aren't normalized, and not to worry about stride or pointer
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*8, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*8, (void*)(16));
  //Draw triangles, going 3 points in
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glUseProgram(0);

  glutSwapBuffers();

  glutPostRedisplay();
}

int init_resources() {
  initialize_program();
  init_vertex_buffers();
  return 1;
}

void free_resources() {
}

int main(int argc, char* argv[]) {
  cout << "Hello, triangle." << endl;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(640, 480);
  glutCreateWindow("Triangle!");

  const GLenum glew_status = glewInit();
  if(GLEW_OK != glew_status) {
    cerr << "Error: " << glewGetErrorString(glew_status) << endl;
    return EXIT_FAILURE;
  }

  if(1 == init_resources()) {
    glutDisplayFunc(render);
    glutMainLoop();
  }

  free_resources();

  return EXIT_SUCCESS;
}
