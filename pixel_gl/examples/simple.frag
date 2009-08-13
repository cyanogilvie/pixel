uniform sampler2D	Tex0;
uniform vec4		col;

void main(void) {
	// this is bug
	gl_FragColor = texture2D(Tex0, vec2(gl_TexCoord[0]));
}


