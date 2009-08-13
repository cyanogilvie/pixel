varying vec3 pos;
varying float calced_y;

void main(void)
{
	//gl_FragColor = vec4(pos.x, pos.y, pos.z, 1.0);
	gl_FragColor = vec4(pos.x, calced_y, pos.z, 1.0);
}
