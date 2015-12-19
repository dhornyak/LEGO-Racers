#version 130

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// színtér tulajdonságok
uniform vec3 eye_pos;
uniform int light;
uniform float periodical_multiplier;
uniform vec3 car_position;

// fénytulajdonságok
vec3 light_pos, toLight;
vec4 La, Ld, Ls;

// anyagtulajdonságok
vec4 Ka, Kd, Ks;
float specular_power = 16.0f;

uniform sampler2D texImage;

void main()
{
	//
	// Set light options.
	//

	if (light == 1)
	{
		// LightOptions::EDITING

		light_pos = vec3( 0, 15, 15 );
		toLight = normalize(light_pos - vs_out_pos);

		La = vec4(0.5f, 0.5f, 0.5f, 1);
		Ld = vec4(0.5f, 0.5f, 0.5f, 1);
		Ls = vec4(1, 1, 1, 1);

		Ka = vec4(1, 1, 1, 1);
		Kd = vec4(0.75f, 0.75f, 0.75f, 1);
		Ks = vec4(1, 1, 1, 1);
	}
	else if (light == 2)
	{
		// LightOptions::RACING_DAY

		light_pos = vec3( 0, 15, 15 );
		toLight = vec3(-1, 1, 0);

		La = vec4(0.5f, 0.5f, 0.5f, 1);
		Ld = vec4(0.5f, 0.5f, 0.5f, 1);
		Ls = vec4(1, 1, 0, 1);

		Ka = vec4(1, 1, 1, 1);
		Kd = vec4(0.75f, 0.75f, 0.75f, 1);
		Ks = vec4(1, 1, 1, 1);
	}
	else if (light == 3)
	{
		// LightOptions::RACING_NIGHT

		light_pos = vec3( 0, 15, -30 );
		toLight = normalize(light_pos - vs_out_pos);

		La = vec4(0.1f, 0.1f, 0.1f, 1);
		Ld = vec4(0.7f, 0.7f, 0.7f, 1);
		Ls = vec4(0, 1, 1, 1);

		Ka = vec4(1, 1, 1, 1);
		Kd = vec4(0.75f, 0.75f, 0.75f, 1);
		Ks = vec4(1, 1, 1, 1);
		specular_power = 2;
	}
	else if (light == 4)
	{
		// LightOptions::FINISH

		light_pos = car_position;
		light_pos.y += 5.0f;
		toLight = normalize(light_pos - vs_out_pos);

		La = vec4(0.2f, 0.2f, 0.2f, 1);
		Ld = vec4(0.5f, 0.5f, 0.5f, 1);
		Ls = vec4(1, 0, 0, 1);

		Ka = vec4(1, 1, 1, 1);
		Kd = vec4(0.75f, 0.75f, 0.75f, 1);
		Ks = vec4(1, 1, 1, 1);
		specular_power = 8.0f * periodical_multiplier;
	}

	//
	// ambiens szín számítása
	//
	vec4 ambient = La * Ka;

	//
	// diffúz szín számítása
	//

	/* segítség:
		- normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	vec3 normal = normalize( vs_out_normal );
	// vec3 toLight = normalize(light_pos - vs_out_pos);
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );
	vec4 diffuse = Ld*Kd*di;

	//
	// fényfoltképzõ szín
	//

	/* segítség:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	vec4 specular = vec4(0);

	if ( di > 0 )
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		vec3 r = reflect( -toLight, normal );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );
		specular = Ls*Ks*si;
	}

	fs_out_col = (ambient + diffuse + specular) * texture(texImage, vs_out_tex0.st);
}