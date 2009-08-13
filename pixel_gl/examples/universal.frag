uniform sampler2D baseAndGloss;
uniform sampler2D normalAndHeight;
uniform vec2 scaleBias;
varying vec3 eyeVec;
varying vec3 lightVec;

void main()
{
	//------------- Compute displaced texture coordinates----------
	float	height		= texture2D(normalAndHeight , gl_TexCoord[0].st).a;
	float	v			= height * scaleBias.r - scaleBias.g;
	vec3	eye			= normalize(eyeVec);
	vec2	newCoords	= gl_TexCoord[0].st + (v * eye.xy);

	//------------- End of compute displaced texture coordinates-----
	vec3	norm		= texture2D(normalAndHeight, newCoords).rgb * 2.0 - 1.0;
	vec4	baseColor	= texture2D(baseAndGloss, newCoords);
	float	dist		= length(lightVec);
	vec3	lightVector	= normalize(lightVec);
	float	nxDir		= max(0.0, dot(norm, lightVector));
	vec4	diffuse		= gl_LightSource[0].diffuse * nxDir;
	float	specularPower = 0.0;
	if (nxDir != 0.0) {
		vec3	cameraVector	= eyeVec;
		vec3	halfVector		= normalize(lightVector + cameraVector);
		float	nxHalf			= max(0.0, dot(norm, halfVector));
				specularPower	= pow(nxHalf, gl_FrontMaterial.shininess);
	}
	vec4	specular			= gl_LightSource[0].specular * specularPower;

	gl_FragColor = (diffuse * vec4(baseColor.rgb, 1.0)) +
		vec4(specular.rgb * baseColor.a, 1.0);
}

