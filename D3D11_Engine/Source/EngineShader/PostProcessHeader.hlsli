
Texture2D T_Albedo : register(t0); //Albedo rgb
Texture2D T_Specular : register(t1); //Specular.r + Metallic.g + Roughness.b + AO.a
Texture2D T_Normal : register(t2); //Normal.rgb
Texture2D T_Emissive : register(t3); //Emissive.rgb
Texture2D T_GBuffer[4] : register(t4); // GBuffer
Texture2D T_Depth : register(t8); //Depth.r
Texture2D T_Input : register(t9); //Input

/** ��� ��ó�� ������ �� �ؽ�ó�� ����մϴ�. */
RWTexture2D<float4> T_Output : register(u0);


