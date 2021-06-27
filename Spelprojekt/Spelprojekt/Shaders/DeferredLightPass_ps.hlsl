#include "Lightning.hlsl"

// G - BUFFER
Texture2D albedoTexture: register(t0);
Texture2D lightTexture: register(t1);
Texture2D normalTexture: register(t2);
Texture2D positionTexture: register(t3);

// SSAO + SHADOW
Texture2D ssaoTexture : register(t4);
Texture2D shadowTexture: register(t5);

SamplerState samplerState: register(s0);

static float2 poissonDiskSamples[16] =
{
	float2(0.7952771f, 0.8865569f),
	float2(0.05550706f, 0.4227468f),
	float2(0.6396513f, 0.4324232f),
	float2(-0.0771156f, 0.8869811f),
	float2(0.8800477f, 0.05700803f),
	float2(0.3543596f, -0.2346418f),
	float2(0.5773101f, -0.6936153f),
	float2(-0.1994669f, -0.3151286f),
	float2(-0.01460677f, -0.9079273f),
	float2(0.9680309f, -0.4074859f),
	float2(0.9791155f, -0.9828652f),
	float2(-0.6393556f, -0.8822003f),
	float2(-0.7635292f, -0.2459009f),
	float2(-0.9553224f, 0.6004817f),
	float2(-0.6042864f, 0.2452013f),
	float2(-0.719184f, 0.9991779f)
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

// https://gamedev.stackexchange.com/questions/32681/random-number-hlsl
/*
	https://web.archive.org/web/20140224205853/http://obge.paradice-insight.us/wiki/Includes_(Effects)
	källa <- död länk
*/
int GetRandomIndex(float4 seed, int maxValue)
{
	float dot_product = dot(seed, float4(12.9898f, 78.233f, 45.164f, 94.673f));
	return int(float(maxValue) * frac(sin(dot_product) * 43758.5453f)) % maxValue;
}

float4 main(PixelInputType input) : SV_TARGET
{
	float4 position = positionTexture.Sample(samplerState, input.uv);
	float shininess = position.a;

	if (position.w == 0.0f)
		discard;

	float4 albedo = albedoTexture.Sample(samplerState, input.uv);
	float3 normal = normalTexture.Sample(samplerState, input.uv).xyz;
	normal = normalize(normal);

	// DIRECTION DATA
	float3 viewDirection = normalize(eyePosition - position.xyz);
	float3 sunDir = normalize(-sunDirection);

	// diffuse shading
	float diff = saturate(dot(float3(0,1,0), sunDir));

	// specular shading
	float3 reflectDir = reflect(-sunDir, normal);

	// SHINYNESS
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), shininess);
	float4 sun_color = sunColor * sunIntensity;

	float4 ambient = albedo * sunIntensity;
	float4 diffuse = albedo * sun_color * diff;
	float4 specular = albedo * sun_color * spec;

	//diffuse = round(diffuse * 5) / 5;

	float4 color = ambient + diffuse;
	for (int i = 0; i < pointLightCount; i++)
	{
		color += CalculatePointLight(pointLights[i], normal, position, viewDirection);
	}

	/*
		SSAO
	*/
	const int blurSize = 4;
	float2 texel = 1.0f / screenSize;

	float ssaoResult = 0.0f;
	float hlim = float(-blurSize) * 0.5f + 0.5f;

	for (int x = 0; x < blurSize; x++)
	{
		for (int y = 0; y < blurSize; y++)
		{
			float2 offset = (hlim + float2(x, y)) * texel;
			ssaoResult += ssaoTexture.Sample(samplerState, input.uv + offset).x;
		}
	}

	ssaoResult = ssaoResult / float(blurSize * blurSize);


	/*
		SHADOWS
	*/
	const int POSSION_DISK_SAMPLE_COUNT = 16;
	const float bias = 0.00001f;
	const float PCFSpread = 1.0f / 1024; //texel size av shadow mapp

	float4 relativeViewPos = mul(mul(position, sunView), sunProjection);

	float2 projectTexCoord;
	projectTexCoord.x = 0.5f + (relativeViewPos.x / relativeViewPos.w) / 2.0f;
	projectTexCoord.y = 0.5f - relativeViewPos.y / relativeViewPos.w / 2.0f;

	float visibility = 1.0f;

	// kollar om den transformerade positionen är innanför sol kamerans vy
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// positionens djup relativt till solens kamera
		float lightDepthValue = (relativeViewPos.z / relativeViewPos.w) - bias;

		[unroll(POSSION_DISK_SAMPLE_COUNT)]
		for (int i = 0; i < POSSION_DISK_SAMPLE_COUNT; i++)
		{
			float4 seed = float4(input.uv.xyy, i);
			int index = GetRandomIndex(seed, 16);
			float sampledDepthValue = shadowTexture.Sample(samplerState, projectTexCoord + (poissonDiskSamples[index] * PCFSpread)).r;

			// om positionens djup är större än skuggans djup som vi samplar så reducerar vi visibility variabeln
			if (lightDepthValue > sampledDepthValue)
			{
				visibility -= (1.0f / POSSION_DISK_SAMPLE_COUNT);
			}
		}
	}

	const float minimumVisibility = 0.3f;
	visibility = saturate(minimumVisibility + (visibility * ssaoResult));
	color *= (visibility);

	// apply gamma translation
	const float gamma = 1.0f / 2.2f;
	color = pow(color, gamma);
	color.w = 1.0f;
	return color;
}