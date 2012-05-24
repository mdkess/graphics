#version 330

layout(location = 0) in vec4 position;
//layout(location = 1) in vec4 color;

uniform float time;
uniform mat4 perspective_matrix;

smooth out vec4 out_color;

void main() {
  const float loop_duration = 5.0f;
  float current_time = mod(time, loop_duration);
  float scale = 3.14159f * 2.0f / loop_duration;

  vec4 offset = vec4(
    cos(current_time * scale) * 0.5f,
    sin(current_time * scale) * 0.5f,
    0.0f, 0.0f);

  gl_Position = perspective_matrix * (position + offset);
  out_color = vec4(1,1,1,1);
  //gl_Position = position + offset;
  //out_color = color;
}
