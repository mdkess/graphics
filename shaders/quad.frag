#version 330

smooth in vec4 out_color;

uniform float time;

out vec4 output_color;

void main() {

  const float loop_duration = 10.0f;
  float current_time = mod(time, loop_duration);
  output_color = mix(vec4(1,1,1,1), out_color, current_time);
}
