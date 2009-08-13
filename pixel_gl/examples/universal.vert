attribute vec3 tangent;
attribute vec3 binormal;
uniform vec3 CAMERA_POSITION;
varying vec3 eyeVec;
varying vec3 lightVec;

void main()
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	//----------------- Compute TBN Matrix ------------------
	mat3 TBNMatrix = mat3(tangent, binormal, gl_Normal);

	//----------------- Compute Eye vector ---------------
	eyeVec = CAMERA_POSITION - gl_Vertex.xyz;
	eyeVec *= TBNMatrix;

	//----------------- Compute Light vector ---------------
	lightVec = gl_LightSource[0].position.xyz - gl_Vertex.xyz;
	lightVec *= TBNMatrix;
}

