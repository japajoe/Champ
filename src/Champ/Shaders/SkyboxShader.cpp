#include "SkyboxShader.hpp"

namespace Champ
{
    static std::string gVertexSource = R"(layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uSunPosition;
uniform float uRayleigh;
uniform float uTurbidity;
uniform float uMieCoefficient;

const vec3 up = vec3(0.0, 1.0, 0.0);

out vec3 FragPosition;
out vec2 TexCoords;
out vec3 SunDirection;
out float SunFade;
out vec3 BetaR;
out vec3 BetaM;
out float SunE;

// constants for atmospheric scattering
const float e = 2.71828182845904523536028747135266249775724709369995957;
const float pi = 3.141592653589793238462643383279502884197169;

// wavelength of used primaries, according to preetham
const vec3 lambda = vec3( 680E-9, 550E-9, 450E-9 );
// this pre-calculation replaces older TotaluRayleigh(vec3 lambda) function:
// (8.0 * pow(pi, 3.0) * pow(pow(n, 2.0) - 1.0, 2.0) * (6.0 + 3.0 * pn)) / (3.0 * N * pow(lambda, vec3(4.0)) * (6.0 - 7.0 * pn))
const vec3 totaluRayleigh = vec3( 5.804542996261093E-6, 1.3562911419845635E-5, 3.0265902468824876E-5 );

// mie stuff
// K coefficient for the primaries
const float v = 4.0;
const vec3 K = vec3( 0.686, 0.678, 0.666 );
// MieConst = pi * pow( ( 2.0 * pi ) / lambda, vec3( v - 2.0 ) ) * K
const vec3 MieConst = vec3( 1.8399918514433978E14, 2.7798023919660528E14, 4.0790479543861094E14 );

// earth shadow hack
// cutoffAngle = pi / 1.95;
const float cutoffAngle = 1.6110731556870734;
const float steepness = 1.5;
const float EE = 1000.0;

float sunIntensity( float zenithAngleCos ) {
	zenithAngleCos = clamp( zenithAngleCos, -1.0, 1.0 );
	return EE * max( 0.0, 1.0 - pow( e, -( ( cutoffAngle - acos( zenithAngleCos ) ) / steepness ) ) );
}

vec3 totalMie( float T ) {
	float c = ( 0.2 * T ) * 10E-18;
	return 0.434 * c * MieConst;
}

void main() {
    FragPosition = vec3(uModel * vec4(aPos, 1.0));
	TexCoords = aTexCoords;
	gl_Position = uProjection * mat4(mat3(uView)) * vec4(aPos, 1.0);
	gl_Position.z = gl_Position.w;

	SunDirection = normalize( uSunPosition );

	SunE = sunIntensity( dot( SunDirection, up ) );

	SunFade = 1.0 - clamp( 1.0 - exp( ( uSunPosition.y / 450000.0 ) ), 0.0, 1.0 );

	float uRayleighCoefficient = uRayleigh - ( 1.0 * ( 1.0 - SunFade ) );

	// extinction (absorption + out scattering)
	// uRayleigh coefficients
	BetaR = totaluRayleigh * uRayleighCoefficient;

	// mie coefficients
	BetaM = totalMie( uTurbidity ) * uMieCoefficient;
})";

static std::string gFragmentSource = R"(layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightnessColor;

uniform float uTime;
uniform float uMieDirectionalG;
uniform float uCloudScale;
uniform float uCloudSpeed;
uniform float uCloudCoverage;
uniform float uCloudDensity;
uniform float uCloudElevation;
uniform float uExposure;

in vec3 FragPosition;
in vec2 TexCoords;
in vec3 SunDirection;
in float SunFade;
in vec3 BetaR;
in vec3 BetaM;
in float SunE;

// constants for atmospheric scattering
const float pi = 3.141592653589793238462643383279502884197169;

const float n = 1.0003; // refractive index of air
const float N = 2.545E25; // number of molecules per unit volume for air at 288.15K and 1013mb (sea level -45 celsius)

// optical length at zenith for molecules
const float rayleighZenithLength = 8.4E3;
const float mieZenithLength = 1.25E3;
// 66 arc seconds -> degrees, and the cosine of that
const float sunAngularDiameterCos = 0.999956676946448443553574619906976478926848692873900859324;

// 3.0 / ( 16.0 * pi )
const float THREE_OVER_SIXTEENPI = 0.05968310365946075;
// 1.0 / ( 4.0 * pi )
const float ONE_OVER_FOURPI = 0.07957747154594767;

const vec3 up = vec3(0.0, 1.0, 0.0);

float rayleighPhase( float cosTheta ) {
	return THREE_OVER_SIXTEENPI * ( 1.0 + pow( cosTheta, 2.0 ) );
}

float hgPhase( float cosTheta, float g ) {
	float g2 = pow( g, 2.0 );
	float inverse = 1.0 / pow( 1.0 - 2.0 * g * cosTheta + g2, 1.5 );
	return ONE_OVER_FOURPI * ( ( 1.0 - g2 ) * inverse );
}

vec4 tone_map_exposure(vec4 color, float exposure) {
    vec3 mapped = vec3(1.0) - exp(-color.rgb * exposure);
    return vec4(mapped, color.a);
}

// Cloud noise functions
float hash( vec2 p ) {
	return fract( sin( dot( p, vec2( 127.1, 311.7 ) ) ) * 43758.5453123 );
}

float noise( vec2 p ) {
	vec2 i = floor( p );
	vec2 f = fract( p );
	f = f * f * ( 3.0 - 2.0 * f );
	float a = hash( i );
	float b = hash( i + vec2( 1.0, 0.0 ) );
	float c = hash( i + vec2( 0.0, 1.0 ) );
	float d = hash( i + vec2( 1.0, 1.0 ) );
	return mix( mix( a, b, f.x ), mix( c, d, f.x ), f.y );
}

float fbm( vec2 p ) {
	float value = 0.0;
	float amplitude = 0.5;
	for ( int i = 0; i < 5; i ++ ) {
		value += amplitude * noise( p );
		p *= 2.0;
		amplitude *= 0.5;
	}
	return value;
}

void main() {
	vec3 direction = normalize( FragPosition  );

	// optical length
	// cutoff angle at 90 to avoid singularity in next formula.
	float zenithAngle = acos( max( 0.0, dot( up, direction ) ) );
	float inverse = 1.0 / ( cos( zenithAngle ) + 0.15 * pow( 93.885 - ( ( zenithAngle * 180.0 ) / pi ), -1.253 ) );
	float sR = rayleighZenithLength * inverse;
	float sM = mieZenithLength * inverse;

	// combined extinction factor
	vec3 Fex = exp( -( BetaR * sR + BetaM * sM ) );

	// in scattering
	float cosTheta = dot( direction, SunDirection );

	float rPhase = rayleighPhase( cosTheta * 0.5 + 0.5 );
	vec3 betaRTheta = BetaR * rPhase;

	float mPhase = hgPhase( cosTheta, uMieDirectionalG );
	vec3 betaMTheta = BetaM * mPhase;

	vec3 Lin = pow( SunE * ( ( betaRTheta + betaMTheta ) / ( BetaR + BetaM ) ) * ( 1.0 - Fex ), vec3( 1.5 ) );
	Lin *= mix( vec3( 1.0 ), pow( SunE * ( ( betaRTheta + betaMTheta ) / ( BetaR + BetaM ) ) * Fex, vec3( 1.0 / 2.0 ) ), clamp( pow( 1.0 - dot( up, SunDirection ), 5.0 ), 0.0, 1.0 ) );

	// nightsky
	float theta = acos( direction.y ); // elevation --> y-axis, [-pi/2, pi/2]
	float phi = atan( direction.z, direction.x ); // azimuth --> x-axis [-pi/2, pi/2]
	vec2 uv = vec2( phi, theta ) / vec2( 2.0 * pi, pi ) + vec2( 0.5, 0.0 );
	vec3 L0 = vec3( 0.1 ) * Fex;

	// composition + solar disc
	float sundisk = smoothstep( sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta );
	L0 += ( SunE * 19000.0 * Fex ) * sundisk;
	
	vec3 texColor = ( Lin + L0 ) * 0.04 + vec3( 0.0, 0.0003, 0.00075 );

	if ( direction.y > 0.0 && uCloudCoverage > 0.0 ) {
        // Project to cloud plane
        float elevation = mix( 1.0, 0.1, uCloudElevation );
        vec2 cloudUV = direction.xz / ( direction.y * elevation );
        cloudUV *= uCloudScale;
        cloudUV += uTime * uCloudSpeed;

        // Multi-octave noise for fluffy clouds
        float cloudNoise = fbm( cloudUV * 1000.0 );
        cloudNoise += 0.5 * fbm( cloudUV * 2000.0 + 3.7 );
        cloudNoise = cloudNoise * 0.5 + 0.5;

        // Apply coverage threshold
        float cloudMask = smoothstep( 1.0 - uCloudCoverage, 1.0 - uCloudCoverage + 0.3, cloudNoise );

        // Fade clouds near horizon
        float horizonFade = smoothstep( 0.0, 0.1 + 0.2 * uCloudElevation, direction.y );
        cloudMask *= horizonFade;

        // --- FIXED CLOUD LIGHTING ---
        
        float sunInfluence = dot( direction, SunDirection ) * 0.5 + 0.5;
        
        // Daylight intensity from sun
        float daylight = max( 0.0, SunDirection.y * 2.0 );
        
        // Nightlight intensity (ambient moon/star glow)
        // This ensures clouds stay visible when daylight is 0.0
        float nightLight = smoothstep(0.2, -0.5, SunDirection.y) * 0.1;

        // Combine sun brightness and a minimum night brightness
        // We use a constant small value so clouds are never 100% black
        float totalIrradiance = max(SunE * 0.00002, 0.000005 + nightLight);

        // Base cloud color 
        vec3 atmosphereColor = Lin * 0.04;
        vec3 cloudBaseColor = mix( vec3( 0.1, 0.1, 0.15 ), vec3( 1.0 ), daylight );
        
        // Apply sun/atmosphere influence
        vec3 cloudColor = mix( cloudBaseColor, atmosphereColor + vec3( 1.0 ), sunInfluence * 0.5 );
        
        // Apply the combined light intensity
        cloudColor *= totalIrradiance;

        // Blend clouds with sky
        // Clouds will now occlude the stars because they are mixed into texColor last
        texColor = mix( texColor, cloudColor, cloudMask * uCloudDensity );
    }
	
	vec4 outputColor = vec4(texColor, 1.0);
	
	// Prevents sky getting completely dark
	//vec3 retColor = pow( texColor, vec3( 1.0 / ( 1.2 + ( 1.2 * SunFade ) ) ) );
	//vec4 outputColor = vec4(retColor, 1.0);

	outputColor = tone_map_exposure(outputColor, uExposure);

	FragColor = outputColor;    
})";

    std::string SkyboxShader::GetVertexSource()
    {
        return gVertexSource;
    }

    std::string SkyboxShader::GetFragmentSource()
    {
        return gFragmentSource;
    }
}