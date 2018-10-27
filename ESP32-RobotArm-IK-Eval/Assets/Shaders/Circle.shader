// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Circle.cs
//  Author: T-Kuhn.
//  Sapporo, September, 2018. Released into the public domain.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Shader "Unlit/Circle"
{
	Properties
	{
		_MainTex ("Texture", 2D) = "white" {}
		_Color ("Tint", Color) = (1,1,1,1)
		_Radius ("Radius", Range(0.0, 1.0)) = 0.0
		_Width ("Width", Range(0.0, 1.0)) = 0.0
	}
	SubShader
	{
		Tags { "RenderType"="Transparent" "Queue" = "Transparent"}
		LOD 100
		Blend SrcAlpha OneMinusSrcAlpha
		Cull Off
		ZWrite Off

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			// make fog work
			#pragma multi_compile_fog
			
			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};

			struct v2f
			{
				float2 uv : TEXCOORD0;
				UNITY_FOG_COORDS(1)
				float4 vertex : SV_POSITION;
			};

			sampler2D _MainTex;
			float4 _MainTex_ST;
			float4 _Color;
			float _Radius;
			float _Width;
			
			v2f vert (appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.uv = TRANSFORM_TEX(v.uv, _MainTex);
				UNITY_TRANSFER_FOG(o,o.vertex);
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target
			{
				fixed4 col = tex2D(_MainTex, i.uv);
				float2 uv = i.uv - float2(0.5,0.5);
				
				float observedRadius = sqrt(uv.x*uv.x + uv.y*uv.y);

				float circle = smoothstep((_Radius - _Width), _Radius, observedRadius);
				circle *= smoothstep((_Radius + _Width), (_Radius), observedRadius);

				// we want a thing to multiply with the color if the pixelposition is within circle range.
				col *= _Color;
				col.a *= circle;
				UNITY_APPLY_FOG(i.fogCoord, col);
				return col;
			}
			ENDCG
		}
	}
}
