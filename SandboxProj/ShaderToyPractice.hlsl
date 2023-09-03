#include "Common.hlsli"
float dLimit = 0.00001;
float dFar = 80.;

#define PI 3.14159265

#define M_SKY        0.
#define M_FLOOR      1.
#define M_B          2.
#define M_BFOOT  	 3.
#define M_BEYE  	 4.
#define M_BHAT  	 5.
#define M_BFIL  	 6.
#define M_BKEY  	 7.
#define M_TER  		 8.

#define mmin( d, n, m ) ( n < d.x ) ? float2( n, m ) : d;

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

cbuffer ActorConstants : register(b10)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT;
    float3 indexColor;
    float time;
};

//float beat1 = 0.0;
//float beat2 = 0.0;

float smin(float a, float b, float k)
{
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

float rand(float n)
{
   // return fract(sin(n) * 43758.5453123);
    return frac(sin(n) * 43758.5453123);
}
float3 noise3(float n)
{
    return float3(rand(n), rand(n * .520 + 1.546), rand(n * 3.10 - 0.56));
}
float3 noise33(float3 n)
{
    return float3(rand(n.x), rand(n.y), rand(n.z));
}

float box(float3 p, float3 b, float r)
{
    float3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float2x2 rot(float a)
{
    float c = cos(a);
    float s = sin(a);
    return float2x2(c, s, -s, c);
}

float ell(float3 p, float3 r)
{
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0) / k1;
}



float rc(float3 p, float r1, float r2, float h)
{
    float b = (r1 - r2) / h;
    float a = sqrt(1.0 - b * b);
    float2 q = float2(length(p.xz), p.y);
    float k = dot(q, float2(-b, a));
    if (k < 0.0)
        return length(q) - r1;
    if (k > a * h)
        return length(q - float2(0.0, h)) - r2;
    return dot(q, float2(a, b)) - r1;
}

float cyl(float3 p, float h, float r)
{
    float2 d = abs(float2(length(p.xz), p.y)) - float2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float2 map(float3 p, float beat1, float beat2);

float3 grad(in float3 p, float beat1, float beat2)
{
    float3 e = float3(0.01, 0.0, 0.0);
    return normalize(float3(
		map(p + e.xyy, beat1, beat2).x - map(p - e.xyy, beat1, beat2).x,
		map(p + e.yxy, beat1, beat2).x - map(p - e.yxy, beat1, beat2).x,
		map(p + e.yyx, beat1, beat2).x - map(p - e.yyx, beat1, beat2).x
	));
}

float4 rm(float3 ro, float3 rd, out float st, float beat1, float beat2)
{
    float3 p = ro;
    float d;
    float2 dmat = float2(100000.0, M_SKY);
    st = 1.;
    for (float i = 0.; i < 150.; i++)
    {
        dmat = map(p, beat1, beat2);
        d = distance(ro, p);
        if (abs(dmat.x) < dLimit || d > dFar)
        {
            st = i / 150.;
            break;
        }
        p += rd * dmat.x;
    }
    return float4(p, dmat.y);
}


float3 fogged(float3 c, float f)
{
    return lerp(c, float3(.529, .808, .922), f);
}
float foggedR(float r, float f)
{
    return lerp(r, 0.0, f);
}


float shade(out float3 c, float m, float3 o, float3 p, float st, float3 n, float3 rd)
{
	
    float f = 1.0 - exp(-0.1 * distance(o, p));

    if (m < M_SKY + 0.5)
    {
        c = fogged(float3(.529, .808, .922), f);
        return foggedR(.0, f);
    }
    else if (m < M_FLOOR + 0.5)
    {
        c = fogged(float3(.5, .5, .5), f);
        return foggedR(0.5, f);
    }
    else if (m < M_B + 0.5)
    {
        c = fogged(float3(.01, .01, .01), f);
        c =c* st;
		//if ( track_time < 69.05 ) {
        return foggedR(0.2, f);
		//} else {
		//	c = mix( c, n * .5 + .5, 0.1);
		//	return foggedR( 0.05, f);
		//}
    }
    else if (m < M_BFOOT + 0.5)
    {
        c = .8 * (1. - st) * fogged(float3(1., .5, .0), f);
        return 0.;
    }
    else if (m < M_BEYE + 0.5)
    {
        c = fogged(float3(1., 1., 1.0), f);
        return foggedR(0.05, f);
    }
    else if (m < M_BHAT + 0.5)
    {
        c = fogged(float3(0., .5, 1.0), f);
        return foggedR(0.5, f);
    }
    else if (m < M_BFIL + 0.5)
    {
        c = fogged(sqrt(st) * lerp(float3(0., 0., 0.), float3(1., 1., 1.), 11. - pow(p.y, 2.)), f);
        return 0.;
    }
    else if (m < M_BKEY + 0.5)
    {
        c = fogged(float3(1., .5, .0), f);
        return foggedR(0.22, f);
    }
    else if (m < M_TER + 0.5)
    {
        c = fogged(st * float3(.5, .5, .5), f);
        return foggedR(0.5, f);
    }
    return .0;
}


float2 bomb(float3 p, float e, float m, float f1, float f2, float r, float kr)
{


    float2 d = float2(10., M_SKY);

    p.y -= 1.5;


    float c = ell(p, float3(1., 1., 1.));

    float3 g = p;
    g.yz = mul(g.yz , rot(-.2 * r));

	// fesses 
	//if( track_time > 69.05 ) {
	//	vec3 pc = g;
	//	pc.y += .5;
	//	pc.z += .6;
	//	pc.x += .2;
	//	c = smin(c, ell( pc, vec3(.35,.4,.2)), .2);
	//	pc.x -= .2*2.;
	//	c = smin(c, ell( pc, vec3(.35,.4,.2)), .2);
	//}
    d = mmin(d, c, M_B);

    float3 pe = g;
    pe.x = -abs(pe.x);
    pe.yz = mul(pe.yz, rot(-0.5));
    pe.zx = mul(pe.yz, rot(-0.4));
    pe = pe - float3(0.0, 0.0, .92);
    d = mmin(d, ell( pe, float3(0.25,0.4 + 0.1 * e,0.1) ), M_BEYE);

	// foots
    float3 pf = p;
    float f = (pf.x > 0.) ? f1 : f2;
    float del = PI / 5.;
    float af = max(-PI / 2., -(f + del * f) + del);
    pf.yz = mul(pf.yz, rot(af));
    pf.x = abs(-pf.x);
    pf.y += 1.0;
    pf.x -= 0.6 - pow(-pf.y, 3.) * .2;
    float df = cyl(pf, .5, 0.2);
    pf.y += .5;
    pf.yz = mul(pf.yz, rot(1.5));
    if (af > .0)
    {
        pf.yz = mul(pf.yz, rot(.3 * (1. - pow(p.y, 2.))));
    }
    df = smin(df, rc(pf, 0.25, 0.3, 0.5), 0.1);
    df = max(df, -0.1 + pf.z);
    d = mmin(d, df, M_BFOOT);
	
	// key
    float3 pk = g;

    pk.yz = mul(pk.yz, rot(1.5));

    pk.zx = mul(pk.zx, rot(-kr * 2. * PI));
    pk.y += 1.;
    float k = cyl(pk, .15, 0.2);
	//pk.z = abs(-pk.z);
    pk.z += .3;
    pk.y += 0.5;
    pk.xy = mul(pk.xy, rot(1.5));
    k = smin(k, max(cyl(pk, .1, .4), -cyl(pk, .3, .2)), 0.2);
    pk.z -= 2. * 0.3;
    k = smin(k, max(cyl(pk, .1, .4), -cyl(pk, .3, .2)), 0.2);

    d = mmin(d, k, M_BKEY);
	

	// Hat
    float3 ph = g;
    ph.y -= .95;
    d = mmin(d, cyl( ph , .1, .45), M_BHAT);

    float3 pi = ph;
    d = mmin(d, cyl( ph + 0.1*float3( ph.y*cos(ph.y * 10.0), .0, (m*3.+2.)*pi.y*pi.y*pi.y) , 1., .1 + 0.01 * pow(cos(200.*pi.y),2.)), M_BFIL);

    return d;
}

PixelShaderOutput main(PixelShaderInput input)
{
    float2 fragCoord = input.texcoord;
    float2 ScreenResolution = float2(1280.0, 720.0); // 예: 1920x1080 해상도

    float beat1 = 0.0;

    float3 color = float3(1., 1.0, 1.0);
    //여기 수정
    //float2 uv = fragCoord / float2(ScreenResolution);
    //uv.x *= float(ScreenResolution.x) / float(ScreenResolution.y);
    //uv = uv * .5 - .5;
    //
    float2 uv = fragCoord;
    float camTime = time * .3;
    float camAmpl = 12. + 2. * sin(time);
    float3 ro = float3(camAmpl * cos(-camTime), 5.5 + 2. * sin(time), camAmpl * sin(-camTime));
    float3 rd = normalize(float3(uv, 0.5)); //rotate_dir(cam_rotation, normalize(vec3(uv,2.0 * cam_fov)));
    rd.xz = mul(rd.xz, rot(-PI / 2. + camTime));
    rd.xy = mul(rd.xy, rot(0.3 * sin(time)));
    float3 n;
    float4 pmat;

    float period = 0.5;

	//for ( float n = 40.0; n < 45.; ++n ) {
	//	beat1 = max(beat1, getNoteVelocity(n,5.) );
	//}

    beat1 = exp(-3.0 * fmod(time, period) / period);
	//beat1 = pow( beat1*2., 2.0);
	//if ( track_time > 28.1 ) {
	//	for ( float n = 35.0; n < 40.; ++n ) {
	//		beat2 = max(beat2, getNoteVelocity(n,5.) );
	//	}
	//	beat2 = pow( beat2*1.5, 2.0);		
	//}


//exp( - 10.0 * mod( sequence_time, period ) / period );
     float beat2 = exp(-3.0 * fmod(time + period * 0.5, period) / period);
	//beat2 = pow( beat2*1.5, 2.0);
	//snare = max(
	//				 max(
	//					max(getNoteVelocity(81.,4.),getNoteVelocity(74.,4.)),
	//				 getNoteVelocity(62.,4.)),
	//				getNoteVelocity(86.,4.));
	
  
    float r = 1.;
    for (int i = 0; i < 3; i++)
    {
        float st;
        pmat = rm(ro, rd, st, beat1, beat2);
        n = grad(pmat.xyz, beat1, beat2);
        float3 cr = n * 0.5 + 0.5;
        r *= shade(cr, pmat.w, ro, pmat.xyz, st, n, rd);

        color *= ((1. - r) * cr.xyz * (1. - st * (r)));
		
        if (r > 0.01)
        {
            ro = pmat.xyz + n * 4. * dLimit;
            rd = reflect(rd, n);
        }
        else
        {
            break;
        }

    }


    color = pow(color, float3(1. / 2.2, 1. / 2.2, 1. / 2.2));
	//color *= 1.0 - smoothstep(71.5, 72.5, track_time);
//color= float3(uv,1.);
    
    PixelShaderOutput output;
    output.pixelColor = float4(color, 1.0);

    return output;
}

float ter(float3 p, float beat1, float beat2)
{
    float per = 6.;
    float3 g = p;
    float3 n = float3(int3((g / per)));
    g.xz = fmod(g.xz, per) - per * .5;

    float t = per * .5;

    t = cyl(g, 10. * rand(n.x), .25 * per * rand(n.y + n.x));
    g = p;
    g.xz = mul(g.xz, rot(.351));
    n = float3(int3((g / per)));

    g.xz = fmod(g.xz, per) - per * .5;
    t = min(t, box(g, float3(.5, 2., .1) + .25 * per * noise3(n.x + n.y), 0.21 * beat1));

    g = p;
    g.xz = mul(g.xz, rot(-.787));
    n = float3(int3((g / per)));
    g.xz = fmod(g.xz, per) - per * .5;
    t = min(t, box(g, float3(.5, 2., .1) + .25 * per * noise3(-n.x + n.y), 0.21 * beat2));
    t = max(t, 13. - length(p));
    return t;
}

float2 map(float3 p, float beat1, float beat2)
{

    float2 d = float2(10., M_SKY);

    d = mmin(d, p.y, M_FLOOR);
    d = mmin(d, ter(p, beat1, beat2), M_TER);

    float3 g = p;
	//if ( track_time > 36.0 ) {
    g.xz = mul(g.xz, rot(time * .8));
	//}

    float per = PI / 4.0;
    //atan -> atan2??
    float a = fmod(atan2(g.z, g.x), per) - .5 * per;
    float l = length(g.zx);

    g.x = l * cos(a);
    g.z = l * sin(a);
	//if ( track_time < 28.1 || track_time > 36.1) {
    g.x -= 7.;
	//} else if ( track_time < 32. ){
	//	g.x -= 7. + 3. * cos(sequence_time*.8);
	//} else {
	//	g.x -= 7. - 3. * cos(sequence_time*.8);
	//}

//if ( track_time < 36.1 ) {
//	g.zx *= rot(-PI/2.);
//}
    float2 b = bomb(g, beat1, beat2, beat1, beat2, max(beat1, beat2), time);
    d = mmin(d, b.x, b.y);

    return d;
}