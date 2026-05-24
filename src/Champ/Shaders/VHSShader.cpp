#include "VHSShader.hpp"

namespace Champ
{
    static std::string gVertexSource = R"(#version 330 core
out vec2 TexCoords;

void main() {
    // Generates a triangle that covers the [-1, 1] range
    // Vertex 0: (-1, -1), UV (0, 0)
    // Vertex 1: ( 3, -1), UV (2, 0)
    // Vertex 2: (-1,  3), UV (0, 2)
    
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    
    TexCoords.x = (x + 1.0) * 0.5;
    TexCoords.y = (y + 1.0) * 0.5;
    
    gl_Position = vec4(x, y, 0.0, 1.0);
})";

    static std::string gFragmentSource = R"(uniform sampler2D uTexture;
uniform float uTime;

in vec2 TexCoords;
out vec4 FragColor;

float noise(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = TexCoords;
    
    // 1. Horizontal jitter/shaking line distortion
    float jitterTime = uTime * 10.0;
    float drift = sin(uv.y * 10.0 + jitterTime) * 0.003;
    float trackingNoise = step(0.05, noise(vec2(uTime, uv.y))) * 0.005;
    
    if (noise(vec2(uTime, uv.y * 20.0)) > 0.98) {
        uv.x += drift + trackingNoise;
    }

    // 2. Center-dependent Chromatic Aberration
    vec2 centerDist = uv - vec2(0.5, 0.5);
    float falloff = dot(centerDist, centerDist);

    float maxOffset = 0.006; 
    float rOffset = maxOffset * sin(uTime * 2.0) * falloff;
    float bOffset = -maxOffset * cos(uTime * 2.0) * falloff;
    float gOffset = 0.0;

    float r = texture(uTexture, vec2(uv.x + rOffset, uv.y)).r;
    float g = texture(uTexture, vec2(uv.x + gOffset, uv.y)).g;
    float b = texture(uTexture, vec2(uv.x + bOffset, uv.y)).b;

    vec3 color = vec3(r, g, b);

    // 3. Scanlines
    float scanline = sin(uv.y * 800.0) * 0.004;
    color -= scanline;

    // 4. Static / Grain Noise
    float grain = (noise(uv + uTime) - 0.5) * 0.025;
    color += grain;

    // 5. Tape Warp / Vignette at edges
    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(16.0 * vignette, 0.25), 0.0, 1.0);
    color *= vignette;

    FragColor = vec4(color, 1.0);
})";

    static std::string gFragmentSource2 = R"(//Source https://www.shadertoy.com/view/XtBXDt
uniform sampler2D uTexture;
uniform float uTime;

in vec2 TexCoords;
out vec4 FragColor;

#define PI 3.14159265

vec3 tex2D( sampler2D _tex, vec2 _p ){
  vec3 col = texture( _tex, _p ).xyz;
  if ( 0.5 < abs( _p.x - 0.5 ) ) {
    col = vec3( 0.1 );
  }
  return col;
}

float hash( vec2 _v ){
  return fract( sin( dot( _v, vec2( 89.44, 19.36 ) ) ) * 22189.22 );
}

float iHash( vec2 _v, vec2 _r ){
  float h00 = hash( vec2( floor( _v * _r + vec2( 0.0, 0.0 ) ) / _r ) );
  float h10 = hash( vec2( floor( _v * _r + vec2( 1.0, 0.0 ) ) / _r ) );
  float h01 = hash( vec2( floor( _v * _r + vec2( 0.0, 1.0 ) ) / _r ) );
  float h11 = hash( vec2( floor( _v * _r + vec2( 1.0, 1.0 ) ) / _r ) );
  vec2 ip = vec2( smoothstep( vec2( 0.0, 0.0 ), vec2( 1.0, 1.0 ), mod( _v*_r, 1. ) ) );
  return ( h00 * ( 1. - ip.x ) + h10 * ip.x ) * ( 1. - ip.y ) + ( h01 * ( 1. - ip.x ) + h11 * ip.x ) * ip.y;
}

float noise( vec2 _v ){
  float sum = 0.;
  for( int i=1; i<9; i++ )
  {
    sum += iHash( _v + vec2( i ), vec2( 2. * pow( 2., float( i ) ) ) ) / pow( 2., float( i ) );
  }
  return sum;
}

void main(){
  vec2 uv = TexCoords;
  vec2 uvn = uv;
  vec3 col = vec3( 0.0 );

  // tape wave
  uvn.x += ( noise( vec2( uvn.y, uTime ) ) - 0.5 )* 0.005;
  uvn.x += ( noise( vec2( uvn.y * 100.0, uTime * 10.0 ) ) - 0.5 ) * 0.01;

  // tape crease
  float tcPhase = clamp( ( sin( uvn.y * 8.0 - uTime * PI * 1.2 ) - 0.92 ) * noise( vec2( uTime ) ), 0.0, 0.01 ) * 10.0;
  float tcNoise = max( noise( vec2( uvn.y * 100.0, uTime * 10.0 ) ) - 0.5, 0.0 );
  uvn.x = uvn.x - tcNoise * tcPhase;

  // switching noise
  float snPhase = smoothstep( 0.03, 0.0, uvn.y );
  uvn.y += snPhase * 0.3;
  uvn.x += snPhase * ( ( noise( vec2( uv.y * 100.0, uTime * 10.0 ) ) - 0.5 ) * 0.2 );
    
  col = tex2D( uTexture, uvn );
  col *= 1.0 - tcPhase;
  col = mix(
    col,
    col.yzx,
    snPhase
  );

  // bloom
  for( float x = -4.0; x < 2.5; x += 1.0 ){
    col.xyz += vec3(
      tex2D( uTexture, uvn + vec2( x - 0.0, 0.0 ) * 7E-3 ).x,
      tex2D( uTexture, uvn + vec2( x - 2.0, 0.0 ) * 7E-3 ).y,
      tex2D( uTexture, uvn + vec2( x - 4.0, 0.0 ) * 7E-3 ).z
    ) * 0.1;
  }
  col *= 0.6;

  // ac beat
  col *= 1.0 + clamp( noise( vec2( 0.0, uv.y + uTime * 0.2 ) ) * 0.6 - 0.25, 0.0, 0.1 );

  FragColor = vec4( col, 1.0 );
})";

    std::string VHSShader::GetVertexSource()
    {
        return gVertexSource;
    }

    std::string VHSShader::GetFragmentSource()
    {
        return gFragmentSource2;
    }
}