varying vec3 pos;
varying float calced_y;
uniform float angle;

void main(void)
{
	pos.x = clamp(gl_Vertex.x, 0.0, 1.0);
	pos.y = clamp(gl_Vertex.y, 0.0, 1.0);
	pos.z = clamp(gl_Vertex.z, 0.0, 1.0);

	calced_y = sin(radians(angle) + gl_Vertex.x + gl_Vertex.z) * 0.5;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position.y += calced_y;
}
